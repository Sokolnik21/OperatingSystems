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

  /* Create server queue */
  if((msgid = msgget(serverQueueKey, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
    if((msgid = msgget(serverQueueKey, IPC_PRIVATE)) == -1) {
      endWithError(-1);
    }
  }

  printf("%d\n", msgid);

  /* Wait for signal */
  while(1) {
    if(msgrcv(msgid, &communicat, sizeof(communicat.mtext), GET_LATEST, MSG_NOERROR) == -1) {
      endWithError(-1);
    }
    printf("%ld\n", communicat.mtype);
    printf("%s\n", communicat.mtext);
  }


  /* Get signal */
  // struct msqid_ds receive;
  struct msgbuf receive;

  // sleep(6);
  //
  // while(1) {
    // int a = msgctl(msgid, IPC_STAT, &receive);
    // printf("%d\n", a);
    // printf("%d\n", receive.mtext);
  //   sleep(1);
  // }
  return 0;
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}
