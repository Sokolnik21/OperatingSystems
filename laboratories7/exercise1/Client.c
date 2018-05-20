#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
// #include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <time.h>

#include "Structures.h"

int * BARBER_SEAT;

void printTime(struct timespec timeCurr, struct timespec timeBegin);
void printClientCommunicat(struct timespec timeBegin, typeOfClientOperation op);

void lockSemaphoreFlag(semaphoreFlags flag, int semid);
void unlockSemaphoreFlag(semaphoreFlags flag, int semid);
void waitForSemaphoreFlag(semaphoreFlags flag, int semid);

void clientWakeUpBarber(struct timespec timeBegin, int semid);
void clientTakeSeatToShave(struct timespec timeBegin, int semid);
void clientLeaveBarberAfterShave(struct timespec timeBegin, int semid);
void clientTakeSeatInQueue(struct timespec timeBegin, int semid);
void clientLeaveBarberWithoutShave(struct timespec timeBegin, int semid);

void endWithError(int n);
void endWithTextError(char * txt, int n);

int main(int argc, char * argv[]) {
  if(argc != 3)
    endWithTextError("Wrong amount of arguments", -1);

  /* Variables */
  struct timespec timeBegin;
  struct timespec * timeBeginShM;
  fifo * QUEUE;
  int * CURR_SEAT;
  int SEATS;
  int WAITING_CLIENTS;
  int shmid;
  int semid;
  int cpid;
  int clients;
  int barberQuan;

  clients = atoi(argv[1]);
  barberQuan = atoi(argv[2]);

  /* Get access to queue */
  /* Variables */
  key_t key;
  /* Set key generator seed */
  char * pathName = malloc(PATH_MAX * sizeof(char));
  pathName = getcwd(pathName, PATH_MAX);
  if((key = ftok(pathName, PROJ_NAME)) == -1)
    endWithError(-1);

  /* Connect shared memory segments */
  if((shmid = shmget(key, 0, 0)) == -1)
    endWithError(-1);
  QUEUE = shmat(shmid, NULL, 0);
  if((shmid = shmget(key + 1, 0, 0)) == -1)
    endWithError(-1);
  CURR_SEAT = shmat(shmid, NULL, 0);
  if((shmid = shmget(key + 2, 0, 0)) == -1)
    endWithError(-1);
  timeBeginShM = (struct timespec *) shmat(shmid, NULL, 0);
  timeBegin = * timeBeginShM;
  if((shmid = shmget(key + 4, 0, 0)) == -1)
    endWithError(-1);
  BARBER_SEAT = shmat(shmid, NULL, 0);

  /* Get semaphore */
  semid = semget(key + 3, 0, 0);
  printf("%d\n", semid);

  // 1. Barber.sleep
  clientWakeUpBarber(timeBegin, semid);
  clientTakeSeatToShave(timeBegin, semid);
  clientLeaveBarberAfterShave(timeBegin, semid);
  // 2. Barber.works
  // if(SEATS == WAITING_CLIENTS)
  //   clientLeaveBarberWithoutShave(timeBegin, semid);
  // else
  //   clientTakeSeatInQueue(timeBegin, semid);


  // sb.sem_num = INVITE;
  // sb.sem_op = -1;
  // sb.sem_flg = 0;
  //
  // if(semop(semid, &sb, 1) == -1)
  //   endWithError(-1);

  /* main loop */
  // while(clients-- > 0) {
  //   /* child */
  //   if(fork() == 0) {
  //     printf("Hi\n");
  //     exit(0);
  //   }
  // //   if(firstElem -> id == 0) {
  // //     // Barber.sleep();
  // //     printf("null\n");
  // //   }
  // //   else {
  // //     // Barber.getOldestClient(queue);
  // //   }
  // }
}


void printTime(struct timespec timeCurr, struct timespec timeBegin) {
  long resultCurr = timeCurr.tv_sec * 10E9 + timeCurr.tv_nsec;
  long resultBegin = timeBegin.tv_sec * 10E9 + timeBegin.tv_nsec;
  long result = resultCurr - resultBegin;
  printf("%lf\n", result / 10E9);
}

void printClientCommunicat(struct timespec timeBegin, typeOfClientOperation op) {
  struct timespec timeCurr;
  clock_gettime(CLOCK_MONOTONIC, &timeCurr);
  switch(op) {
    case OP_WAKE_UP_BARBER :
      printf("Client(%d).wakeUpBarber : ", getpid());
      break;
    case OP_TAKE_SEAT_TO_SHAVE :
      printf("Client(%d).takeSeatToShave : ", getpid());
      break;
    case OP_LEAVE_AFTER_SHAVE :
      printf("Client(%d).leaveAfterShave : ", getpid());
      break;
    case OP_TAKE_SEAT_IN_QUEUE :
      printf("Client(%d).takeSeatInQueue : ", getpid());
      break;
    case OP_LEAVE_WITHOUT_SHAVE :
      printf("Client(%d).leaveWithoutShave : ", getpid());
      break;
  }
  printTime(timeCurr, timeBegin);
}

void lockSemaphoreFlag(semaphoreFlags flag, int semid) {
  struct sembuf sb;
  sb.sem_num = flag;
  sb.sem_op = -1;
  sb.sem_flg = 0;

  if(semop(semid, &sb, 1) == -1)
    endWithError(-1);
}

void unlockSemaphoreFlag(semaphoreFlags flag, int semid) {
  struct sembuf sb;
  sb.sem_num = flag;
  sb.sem_op = 1;
  sb.sem_flg = 0;

  if(semop(semid, &sb, 1) == -1)
    endWithError(-1);
}

void waitForSemaphoreFlag(semaphoreFlags flag, int semid) {
  struct sembuf sb;
  sb.sem_num = flag;
  sb.sem_op = 0;
  sb.sem_flg = 0;

  if(semop(semid, &sb, 1) == -1)
    endWithError(-1);
}

void clientWakeUpBarber(struct timespec timeBegin, int semid) {
  printClientCommunicat(timeBegin, OP_WAKE_UP_BARBER);
  unlockSemaphoreFlag(SLEEP, semid);
  unlockSemaphoreFlag(SLEEP, semid);
  waitForSemaphoreFlag(INVITE, semid);
}

void clientTakeSeatToShave(struct timespec timeBegin, int semid) {
  printClientCommunicat(timeBegin, OP_TAKE_SEAT_TO_SHAVE);
  * BARBER_SEAT = getpid();
  unlockSemaphoreFlag(INVITE, semid);
  unlockSemaphoreFlag(INVITE, semid);
  waitForSemaphoreFlag(FINISH, semid);
}

void clientLeaveBarberAfterShave(struct timespec timeBegin, int semid) {
  printClientCommunicat(timeBegin, OP_LEAVE_AFTER_SHAVE);
  unlockSemaphoreFlag(FINISH, semid);
  unlockSemaphoreFlag(FINISH, semid);
}

void clientTakeSeatInQueue(struct timespec timeBegin, int semid) {
  printClientCommunicat(timeBegin, OP_TAKE_SEAT_IN_QUEUE);
}

void clientLeaveBarberWithoutShave(struct timespec timeBegin, int semid) {
  printClientCommunicat(timeBegin, OP_LEAVE_WITHOUT_SHAVE);
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

void endWithTextError(char * txt, int n) {
  printf("Error: %s\n", txt);
  exit(n);
}
