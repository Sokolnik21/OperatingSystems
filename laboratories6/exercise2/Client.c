#include <mqueue.h>
/* Keys */
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
/* I/O */
#include <stdio.h>

#include <errno.h>
#include <string.h>
/* Constants */
#include "ServerConstants.h"

void endWithError(int n);

int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 2) {
    printf("Wrong amount of arguments\n");
    return -1;
  }

  /* Variables */
  char filePath[PATH_MAX];
  mqd_t queueS;
  mqd_t queueC;
  char * queueCliName = malloc(sizeof(CLIENT_PREFIX) + sizeof(int));
  char * communicat = malloc(MSGMAX * sizeof(char));
  int prio;
  union bufStuff buf;
  char clientId = '0';  // Starting value
  buf.contex.id = clientId;

  /* Parsing variables */
  strcpy(filePath, argv[1]);

  /* Get id of the server queue */
  if((queueS = mq_open(SERVER_PREFIX, O_RDWR, 0600, NULL)) == -1)
    endWithError(-1);

  /* Create client queue */
  sprintf(queueCliName, "%s%d", CLIENT_PREFIX, getpid());
  if((queueC = mq_open(queueCliName, O_RDWR | O_CREAT | O_EXCL, 0600, NULL)) == -1)
    endWithError(-1);

  /* Send client queue id to server */
  buf.contex.type = CLI_QUEUE;
  strcpy(buf.contex.text, queueCliName);
  strcpy(communicat, buf.buffer);
  if(mq_send(queueS, communicat, MSGMAX, 0) == -1)
    endWithError(-1);
  if(mq_receive(queueC, communicat, MSGMAX * 10, &prio) == -1)
    endWithError(-1);
  strcpy(buf.buffer, communicat);
  clientId = atoi(buf.contex.text);
  buf.contex.id = clientId;
  printf("%s. client\n", buf.contex.text);

  /* SEND MESSAGES START */
  /* Setting file handler */
  FILE * f = fopen(filePath, "r");
  if (f == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  /* Get every line from file and parse it */
  char * line = malloc(PATH_MAX * sizeof(char));
  size_t len = 0;
  ssize_t readLine;
  while ((readLine = getline(&line, &len, f)) != -1) {
    switch(line[0]) {
      case 'M' :
        buf.contex.type = MIRROR;
        buf.contex.id = clientId;
        strcpy(buf.contex.text, line + 7);
        strcpy(communicat, buf.buffer);
        break;
      case 'C' :
        buf.contex.type = CALC;
        buf.contex.id = clientId;
        strcpy(buf.contex.text, line + 5);
        strcpy(communicat, buf.buffer);
        break;
      case 'T' :
        buf.contex.type = TIME;
        buf.contex.id = clientId;
        strcpy(communicat, buf.buffer);
        break;
      case 'E' :
        buf.contex.type = END;
        buf.contex.id = clientId;
        strcpy(communicat, buf.buffer);
        break;
    }
    if(mq_send(queueS, communicat, MSGMAX, 0) == -1)
      endWithError(-1);
    if(mq_receive(queueC, communicat, MSGMAX * 10, &prio) == -1)
      endWithError(-1);
    strcpy(buf.buffer, communicat);
    printf("%s\n", buf.contex.text);
    sleep(1);
  }
  /* SEND MESSAGES END */

  /* Close client queue */
  if(mq_close(queueC) == -1)
    endWithError(-1);
  if(mq_unlink(queueCliName) == -1)
    endWithError(-1);

  /* Close client queue */
  if(mq_close(queueS) == -1)
    endWithError(-1);

  return 0;
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}
