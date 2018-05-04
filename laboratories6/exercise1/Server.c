#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
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

struct msgbuf msgCliQue(int iterClients);
struct msgbuf msgMirror(struct msgbuf communicat);
struct msgbuf msgCalc(struct msgbuf communicat);
struct msgbuf msgTime();
void msgEnd(int msgid);

int main(int argc, char * argv[]) {
  /* Variables */
  key_t serverQueueKey;
  int msgid;
  struct msgbuf communicat;
  struct msgbuf respond;
  int pidToMsgid[2][MAX_CLIENTS];
  int iterClients;
  for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
    pidToMsgid[0][iterClients] = -1;

  /* Set key generator seed */
  char * pathName = malloc(PATH_MAX * sizeof(char));
  pathName = getcwd(pathName, PATH_MAX);
  if((serverQueueKey = ftok(pathName, PROJ_NAME)) == -1) {
    endWithError(-1);
  }

  /* Create server queue */
  if((msgid = msgget(serverQueueKey, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
    if((msgid = msgget(serverQueueKey, IPC_PRIVATE)) == -1) {
      endWithError(-1);
    }
  }

  /* Wait for signal */
  while(1) {
    if(msgrcv(msgid, &communicat, sizeof(communicat.mtext), GET_LATEST, MSG_NOERROR) == -1) {
      endWithError(-1);
    }
    switch(communicat.mtype) {
      case CLI_QUEUE :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
          if(pidToMsgid[0][iterClients] == -1) {
            pidToMsgid[0][iterClients] = communicat.pid;
            pidToMsgid[1][iterClients] = atoi(communicat.mtext);

            respond = msgCliQue(iterClients);

            if(msgsnd(pidToMsgid[1][iterClients], &respond, MSGMAX, MSG_NOERROR) == -1) {
              endWithError(-1);
            }
            break;
          }
        break;
      case MIRROR :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
          if(pidToMsgid[0][iterClients] == communicat.pid) {

            respond = msgMirror(communicat);

            if(msgsnd(pidToMsgid[1][iterClients], &respond, MSGMAX, MSG_NOERROR) == -1) {
              endWithError(-1);
            }
          }
        break;
      case CALC :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
          if(pidToMsgid[0][iterClients] == communicat.pid) {

            respond = msgCalc(communicat);

            if(msgsnd(pidToMsgid[1][iterClients], &respond, MSGMAX, MSG_NOERROR) == -1) {
              endWithError(-1);
            }
          }
        break;
      case TIME :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
          if(pidToMsgid[0][iterClients] == communicat.pid) {

            respond = msgTime();

            if(msgsnd(pidToMsgid[1][iterClients], &respond, MSGMAX, MSG_NOERROR) == -1) {
              endWithError(-1);
            }
          }
        break;
      case END :
        for(iterClients = 0; iterClients < MAX_CLIENTS; iterClients++)
          if(pidToMsgid[0][iterClients] == communicat.pid) {

            msgEnd(msgid);

          }
        break;

    }
  }
  return 0;
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

struct msgbuf msgCliQue(int iterClients) {
  struct msgbuf respond;

  respond.mtype = CLI_QUEUE;
  respond.pid = getpid();
  sprintf(respond.mtext, "%d", iterClients);

  return respond;
}

struct msgbuf msgMirror(struct msgbuf communicat) {
  struct msgbuf respond;

  respond.mtype = MIRROR;
  respond.pid = getpid();
  int length = strlen(communicat.mtext);
  int currChar;
  for(currChar = 0; currChar < length; currChar++) {
    respond.mtext[currChar] = communicat.mtext[length - currChar - 1];
  }
  respond.mtext[length] = '\0';

  return respond;
}

struct msgbuf msgCalc(struct msgbuf communicat) {
  struct msgbuf respond;

  respond.mtype = CALC;
  respond.pid = getpid();
  int operand1, operand2, result;
  char sign;
  sscanf(communicat.mtext, "%d%c%d", &operand1, &sign, &operand2);
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
  sprintf(respond.mtext, "%d", result);

  return respond;
}

struct msgbuf msgTime() {
  struct msgbuf respond;

  respond.mtype = TIME;
  respond.pid = getpid();
  time_t timer;
  time(&timer);
  struct tm * tm_info = localtime(&timer);
  strftime(respond.mtext, MSGMAX, "%Y-%m-%d %H:%M:%S", tm_info);

  return respond;
}

void msgEnd(int msgid) {
  if(msgctl(msgid, IPC_RMID, 0) == -1) {
    endWithError(-1);
  }
  exit(0);
}
