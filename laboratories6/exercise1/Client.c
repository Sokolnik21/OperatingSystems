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
/* Constants */
#include "ServerConstants.h"

void endWithError(int n);
void removeQueue(void);

int GLOBAL_CLIENT_MSG_ID;

int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 2) {
    printf("Wrong amount of arguments\n");
    return -1;
  }

  /* Variables */
  char filePath[PATH_MAX];
  key_t serverQueueKey;
  int msgidS;
  int msgidC;
  struct msgbuf communicat;
  struct msgbuf serverMsg;

  /* Parsing variables */
  strcpy(filePath, argv[1]);

  /* Almost-Garbage collector */
  atexit(removeQueue);

  /* Set key generator seed */
  char * pathName = malloc(PATH_MAX * sizeof(char));
  pathName = getcwd(pathName, PATH_MAX);
  if((serverQueueKey = ftok(pathName, PROJ_NAME)) == -1) {
    endWithError(-1);
  }

  /* Get id of the server queue */
  /* Assume that "Server.c" and "Client.c" are in the same catalog */
  if((msgidS = msgget(serverQueueKey, IPC_PRIVATE)) == -1) {
    endWithError(-1);
  }

  /* Create client private queue */
  int nextKey = 0;
  while((msgidC = msgget((serverQueueKey + nextKey) % LONG_MAX, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
    nextKey++;
    if(nextKey == LONG_MAX) {
      endWithError(-1);
    }
  }
  GLOBAL_CLIENT_MSG_ID = msgidC;

  /* Send client queue id to server */
  communicat.mtype = CLI_QUEUE;
  communicat.pid = getpid();
  sprintf(communicat.mtext, "%d", msgidC);
  if(msgsnd(msgidS, &communicat, MSGMAX, MSG_NOERROR) == -1) {
    endWithError(-1);
  }
  if(msgrcv(msgidC, &serverMsg, sizeof(serverMsg.mtext), GET_LATEST, MSG_NOERROR) == -1) {
    endWithError(-1);
  }
  printf("%s. client", serverMsg.mtext);

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
        communicat.mtype = MIRROR;
        strcpy(communicat.mtext, line + 7);
        if(msgsnd(msgidS, &communicat, MSGMAX, MSG_NOERROR) == -1) {
          endWithError(-1);
        }
        break;
      case 'C' :
        communicat.mtype = CALC;
        strcpy(communicat.mtext, line + 5);
        if(msgsnd(msgidS, &communicat, MSGMAX, MSG_NOERROR) == -1) {
          endWithError(-1);
        }
        break;
      case 'T' :
        communicat.mtype = TIME;
        if(msgsnd(msgidS, &communicat, MSGMAX, MSG_NOERROR) == -1) {
          endWithError(-1);
        }
        break;
      case 'E' :
        communicat.mtype = END;
        if(msgsnd(msgidS, &communicat, MSGMAX, MSG_NOERROR) == -1) {
          endWithError(-1);
        }
        break;
    }
    if(msgrcv(msgidC, &serverMsg, sizeof(serverMsg.mtext), GET_LATEST, MSG_NOERROR) == -1) {
      endWithError(-1);
    }
    printf("%s\n", serverMsg.mtext);
    sleep(1);
  }
  /* SEND MESSAGES END */

  return 0;
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

void removeQueue(void) {
  if(msgctl(GLOBAL_CLIENT_MSG_ID, IPC_RMID, 0) == -1) {
    endWithError(-1);
  }
}
