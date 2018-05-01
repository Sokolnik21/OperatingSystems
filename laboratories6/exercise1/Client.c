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

int main(int argc, char * argv[]) {
  /* Variables */
  key_t serverQueueKey;
  int msgid;
  struct msgbuf communicat;

  /* Set key generator seed */
  char * pathName = malloc(PATH_MAX * sizeof(char));
  pathName = getcwd(pathName, PATH_MAX);
  if((serverQueueKey = ftok(pathName, PROJ_NAME)) == -1) {
    endWithError(-1);
  }

  /* Get id of the server queue */
  /* Assume that "Server.c" and "Client.c" are in the same catalog */
  if((msgid = msgget(serverQueueKey, IPC_PRIVATE)) == -1) {
    endWithError(-1);
  }

  /* Send signal */
  communicat.mtype = 2;
  strcpy(communicat.mtext, "Zdravstvuj\n");
  // printf("%s\n", communicat.mtext);
  // communicat.mtext[0] = 'H';
  // communicat.mtext[1] = 'i';
  // msgsnd(IPC_PRIVATE, &communicat, MSGMAX, MSG_NOERROR);
  if(msgsnd(msgid, &communicat, MSGMAX, MSG_NOERROR) == -1) {
    endWithError(-1);
  }


  while(1) {
    sleep(1);
  }
  return 0;
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}
