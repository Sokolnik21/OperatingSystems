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
  /* Variables */
  key_t serverQueueKey;
  int msgidS;
  int msgidC;
  struct msgbuf communicat;
  struct msgbuf serverMsg;

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
  printf("%s. client\n", serverMsg.mtext);

  /* SEND MESSAGES START */
  while(1) {
    /* Send signal and wait for response */
    communicat.mtype = MIRROR;
    communicat.pid = getpid();
    strcpy(communicat.mtext, "14*93");
    strcpy(communicat.mtext, "Ziggy plays guitar");
    if(msgsnd(msgidS, &communicat, MSGMAX, MSG_NOERROR) == -1) {
      endWithError(-1);
    }
    if(msgrcv(msgidC, &serverMsg, sizeof(serverMsg.mtext), GET_LATEST, MSG_NOERROR) == -1) {
      endWithError(-1);
    }
    printf("%s\n", serverMsg.mtext);
    sleep(1);

    exit(0);
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
