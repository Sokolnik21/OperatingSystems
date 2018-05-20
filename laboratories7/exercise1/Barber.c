/**
  Shared memory hints:
  1. It's necessary to unlink and then delete shared memory segments before
    terminating program. If program hasn't done this then that segments
    will stay in system.
    Q: So what? Nowadays programmers should care more about the problem itself
      than implementation.
    A: Well, not in C. In this case that approach leads to hard-to-find-bugs
      (taken from my experience).
      When segment still exists in system it kinda can't be overwritten.
      And You can't change size (or whatever it was) of that shared memory
      segment. In ex. during implementation after You change Your structure that
      stores data program starts to crash.
      But the worst thing is that for compiler it's ok and won't signal this as
      an error.
  2. You cannot use the same key to create semaphore and shared memory.
 */

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
void printBarberCommunicat(struct timespec timeBegin, typeOfBarberOperation op);

void lockSemaphoreFlag(semaphoreFlags flag, int semid);
void unlockSemaphoreFlag(semaphoreFlags flag, int semid);
void waitForSemaphoreFlag(semaphoreFlags flag, int semid);

void barberSleep(struct timespec timeBegin, int semid);
void barberWakeUp(struct timespec timeBegin, int semid);
void barberInvite(struct timespec timeBegin, int semid);
void barberBeginShave(struct timespec timeBegin, int semid);
void barberEndShave(struct timespec timeBegin, int semid);

void endWithError(int n);
void endWithTextError(char * txt, int n);

int main(int argc, char * argv[]) {
  if(argc != 2)
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

  clock_gettime(CLOCK_MONOTONIC, &timeBegin);
  SEATS = atoi(argv[1]);
  QUEUE = malloc(SEATS * sizeof(fifo));

  /* Create queue */
  /* Variables */
  key_t key;
  /* Set key generator seed */
  char * pathName = malloc(PATH_MAX * sizeof(char));
  pathName = getcwd(pathName, PATH_MAX);
  if((key = ftok(pathName, PROJ_NAME)) == -1)
    endWithError(-1);

  /* Create and connect shared memory segments */
  if((shmid = shmget(key, SEATS * sizeof(fifo), 0600 | IPC_CREAT)) == -1)
    endWithError(-1);
  QUEUE = shmat(shmid, NULL, 0);
  if((shmid = shmget(key + 1, sizeof(int), 0600 | IPC_CREAT)) == -1)
    endWithError(-1);
  CURR_SEAT = shmat(shmid, NULL, 0);
  if((shmid = shmget(key + 2, sizeof(struct timespec), 0600 | IPC_CREAT)) == -1)
    endWithError(-1);
  timeBeginShM = (struct timespec *) shmat(shmid, NULL, 0);
  if((shmid = shmget(key + 4, sizeof(int), 0600 | IPC_CREAT)) == -1)
    endWithError(-1);
  BARBER_SEAT = shmat(shmid, NULL, 0);

  /* Initialize shared memory segments */
  int iterQue;
  for(iterQue = 0; iterQue < SEATS; iterQue++) {
    QUEUE[iterQue].id = 0;
    QUEUE[iterQue].invited = false;
  }
  * CURR_SEAT = 0;
  * timeBeginShM = timeBegin;

  /* Create semaphore */
  semid = semget(key + 3, SEMAPHORE_FLAGS, 0600 | IPC_CREAT);
  printf("%d\n", semid);
  /* Initialize semaphore */
  int iterSem;
  for(iterSem = 0; iterSem < SEMAPHORE_FLAGS; iterSem++)
    if(semctl(semid, iterSem, SETVAL, 1) == -1)
      endWithError(-1);

  /* Barber sleep */
  // 1. QUEUE == empty
  barberSleep(timeBegin, semid);
  barberWakeUp(timeBegin, semid);
  barberInvite(timeBegin, semid);
  barberBeginShave(timeBegin, semid);
  barberEndShave(timeBegin, semid);

  sleep(1);

  /* Clear section */
  /* Clear shared memory */
  shmdt(QUEUE);
  shmdt(CURR_SEAT);
  shmdt(timeBeginShM);
  shmdt(BARBER_SEAT);
  shmid = shmget(key, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  shmid = shmget(key + 1, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  shmid = shmget(key + 2, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  shmid = shmget(key + 4, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  /* Clear semaphores */
  semctl(semid, key + 3, IPC_RMID, NULL);

}

void printTime(struct timespec timeCurr, struct timespec timeBegin) {
  long resultCurr = timeCurr.tv_sec * 10E9 + timeCurr.tv_nsec;
  long resultBegin = timeBegin.tv_sec * 10E9 + timeBegin.tv_nsec;
  long result = resultCurr - resultBegin;
  printf("%lf\n", result / 10E9);
}

void printBarberCommunicat(struct timespec timeBegin, typeOfBarberOperation op) {
  struct timespec timeCurr;
  clock_gettime(CLOCK_MONOTONIC, &timeCurr);
  switch(op) {
    case OP_SLEEP :
      printf("Barber.sleep : ");
      break;
    case OP_WAKE_UP :
      printf("Barber.wakeUp : ");
      break;
    case OP_INVITE :
      printf("Barber.inviteClient : ");
      break;
    case OP_BEGIN_SHAVE :
      printf("Barber.beginShave(%d) : ", * BARBER_SEAT);
      break;
    case OP_END_SHAVE :
      printf("Barber.endShave(%d) : ", * BARBER_SEAT);
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

void barberSleep(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_SLEEP);
  lockSemaphoreFlag(SLEEP, semid);
  lockSemaphoreFlag(SLEEP, semid);
}

void barberWakeUp(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_WAKE_UP);
}

void barberInvite(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_INVITE);
  lockSemaphoreFlag(INVITE, semid);
  lockSemaphoreFlag(INVITE, semid);
}

void barberBeginShave(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_BEGIN_SHAVE);
}

void barberEndShave(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_END_SHAVE);
  lockSemaphoreFlag(FINISH, semid);
  lockSemaphoreFlag(FINISH, semid);
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

void endWithTextError(char * txt, int n) {
  printf("Error: %s\n", txt);
  exit(n);
}
