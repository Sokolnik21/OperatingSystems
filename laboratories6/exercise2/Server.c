#include <mqueue.h>
/* Keys */
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
/* I/O */
#include <stdio.h>

#include <errno.h>
#include <string.h>
#include <time.h>
/* Constants */
#include "ServerConstants.h"

void endWithError(int n);

union bufStuff msgCliQue(int iterClients);
union bufStuff msgMirror(char * communicat);
union bufStuff msgCalc(char * communicat);
union bufStuff msgTime();
void msgEnd(int queue, char * name);

int main(int argc, char * argv[]) {
  /* Variables */
  mqd_t queueS;
  mqd_t queueC;
  char * communicat = malloc(MSGMAX * 10 * sizeof(char));
  int prio;
  union bufStuff buf;
  buf.contex.id = '0';  // Starting value

  int cliIdToQueId[2][MAX_CLIENTS];
  int iterClients;
  for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
    cliIdToQueId[0][iterClients] = -1;

  /* Create server queue */
  if((queueS = mq_open(SERVER_PREFIX, O_RDWR | O_CREAT | O_EXCL, 0600, NULL)) == -1)
    if(mq_unlink(SERVER_PREFIX) == -1)
      endWithError(-1);
    else
      if((queueS = mq_open(SERVER_PREFIX, O_RDWR | O_CREAT | O_EXCL, 0600, NULL)) == -1)
        endWithError(-1);

  /* Wait for signal */
  while(1) {
    if(mq_receive(queueS, communicat, MSGMAX * 10, &prio) == -1)
      endWithError(-1);
    strcpy(buf.buffer, communicat);
    switch(buf.contex.type) {
      case CLI_QUEUE :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
          if(cliIdToQueId[0][iterClients] == -1) {
            if((queueC = mq_open(buf.contex.text, O_RDWR, 0600, NULL)) == -1)
              endWithError(-1);
            cliIdToQueId[0][iterClients] = iterClients + 1;
            cliIdToQueId[1][iterClients] = queueC;

            buf = msgCliQue(iterClients + 1); // + 1 to prevent '\0' inside string
            break;
          }
        break;
      case MIRROR :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++) {
          if(cliIdToQueId[0][iterClients] == buf.contex.id) {
            queueC = cliIdToQueId[1][iterClients];
            buf = msgMirror(buf.contex.text);
          }
        }
        break;
      case CALC :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
          if(cliIdToQueId[0][iterClients] == buf.contex.id) {
            queueC = cliIdToQueId[1][iterClients];
            buf = msgCalc(buf.contex.text);
          }
        break;
      case TIME :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
          if(cliIdToQueId[0][iterClients] == buf.contex.id) {
            queueC = cliIdToQueId[1][iterClients];
            buf = msgTime();
          }
        break;
      case END :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
          if(cliIdToQueId[0][iterClients] == buf.contex.id) {
            buf.contex.type = END;
            strcpy(buf.contex.text, "Server will shut down");
            if(mq_send(cliIdToQueId[1][0], buf.buffer, MSGMAX, 0) == -1)
              endWithError(-1);
            msgEnd(queueS, SERVER_PREFIX);
          }
        break;
    }
    buf.contex.id = '0';
    if(mq_send(queueC, buf.buffer, MSGMAX, 0) == -1)
      endWithError(-1);
  }
  return 0;
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

union bufStuff msgCliQue(int iterClients) {
  union bufStuff buf;

  buf.contex.type = CLI_QUEUE;
  sprintf(buf.contex.text, "%d", iterClients);

  return buf;
}

union bufStuff msgMirror(char * communicat) {
  union bufStuff buf;

  buf.contex.type = MIRROR;
  int length = strlen(communicat);
  int currChar;
  for(currChar = 0; currChar < length; currChar++) {
    buf.contex.text[currChar] = communicat[length - currChar - 1];
  }
  buf.contex.text[length] = '\0';

  return buf;
}

union bufStuff msgCalc(char * communicat) {
  union bufStuff buf;

  buf.contex.type = CALC;
  int operand1, operand2, result;
  char sign;
  sscanf(communicat, "%d%c%d", &operand1, &sign, &operand2);
  switch(sign) {
    case '+' :
      result = operand1 + operand2;
      break;
    case '-' :
      result = operand1 - operand2;
      break;
    case '*' :
      result = operand1 * operand2;
      break;
    case '/' :
      result = operand1 / operand2;
      break;
  }
  sprintf(buf.contex.text, "%d", result);

  return buf;
}

union bufStuff msgTime() {
  union bufStuff buf;

  buf.contex.type = TIME;
  time_t timer;
  time(&timer);
  struct tm * tm_info = localtime(&timer);
  strftime(buf.contex.text, MSGMAX, "%Y-%m-%d %H:%M:%S", tm_info);

  return buf;
}

void msgEnd(int queue, char * name) {
  /* Close server queue */
  if(mq_close(queue) == -1)
    endWithError(-1);
  if(mq_unlink(name) == -1)
    endWithError(-1);
  exit(0);
}
