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
#include <signal.h>

#include "Structures.h"

int * BARBER_SEAT;
int * BARBER_SLEEP;
struct timespec * timeBeginShM;
fifo * QUEUE;
int * CURR_SEAT;
int * WAITING_CLIENTS;
int * SEATS_SHM;

void agentSIG_C(int signum);
void clearSemaphoresAndSharedMemory();

void printTime(struct timespec timeCurr, struct timespec timeBegin);
void printBarberCommunicat(struct timespec timeBegin, typeOfBarberOperation op);

void lockSemaphoreFlag(semaphoreFlags flag, int semid);
void unlockSemaphoreFlag(semaphoreFlags flag, int semid);
void waitForSemaphoreFlag(semaphoreFlags flag, int semid);
int tryToLockSemaphoreFlag(semaphoreFlags flag, int semid);

void barberSleep(struct timespec timeBegin, int semid);
void barberWakeUp(struct timespec timeBegin, int semid);
void barberInvite(struct timespec timeBegin, int semid);
void barberBeginShave(struct timespec timeBegin, int semid);
void barberEndShave(struct timespec timeBegin, int semid);

void endWithError(int n);
void endWithTextError(char * txt, int n);

void DBG_printQueue(fifo * QUEUE) {
  printf("///////QUEUE///////\n");
  printf("| ");
  int i;
  for(i = 0; i < * SEATS_SHM; i++) {
      printf("%d | ", QUEUE[i].id);
  }
  printf("\n");
  printf("///////////////////\n");
}

int main(int argc, char * argv[]) {
  if(argc != 2)
    endWithTextError("Wrong amount of arguments", -1);

  /* Variables */
  struct timespec timeBegin;
  int SEATS;
  int shmid;
  int semid;

  /* Signal handler */
  signal(SIGINT, agentSIG_C);

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
  if((shmid = shmget(key + 5, sizeof(int), 0600 | IPC_CREAT)) == -1)
    endWithError(-1);
  WAITING_CLIENTS = shmat(shmid, NULL, 0);
  if((shmid = shmget(key + 6, sizeof(int), 0600 | IPC_CREAT)) == -1)
    endWithError(-1);
  SEATS_SHM = shmat(shmid, NULL, 0);
  if((shmid = shmget(key + 7, sizeof(int), 0600 | IPC_CREAT)) == -1)
    endWithError(-1);
  BARBER_SLEEP = shmat(shmid, NULL, 0);

  /* Initialize shared memory segments */
  int iterQue;
  for(iterQue = 0; iterQue < SEATS; iterQue++) {
    QUEUE[iterQue].id = 0;
    QUEUE[iterQue].invited = false;
  }
  * CURR_SEAT = 0;
  * timeBeginShM = timeBegin;
  * WAITING_CLIENTS = 0;
  * SEATS_SHM = SEATS;
  // * BARBER_SLEEP = true;
  * BARBER_SLEEP = false;

  /* Create semaphore */
  semid = semget(key + 3, SEMAPHORE_FLAGS, 0600 | IPC_CREAT);

  /* Initialize semaphore */
  if(semctl(semid, INVITE, SETVAL, 0) == -1)
    endWithError(-1);
  if(semctl(semid, FINISH, SETVAL, 0) == -1)
    endWithError(-1);
  if(semctl(semid, SLEEP, SETVAL, 0) == -1)
    endWithError(-1);
  if(semctl(semid, LEAVE, SETVAL, 0) == -1)
    endWithError(-1);
  if(semctl(semid, TAKEN_SEAT, SETVAL, 0) == -1)
    endWithError(-1);
  if(semctl(semid, ACCESS_TO_BARBER_SLEEP, SETVAL, 1) == -1)
    endWithError(-1);
  if(semctl(semid, ACCESS_TO_WAITING_CLIENTS, SETVAL, 1) == -1)
    endWithError(-1);

  /* Barber sleep */
  while(true) {
    DBG_printQueue(QUEUE);
    lockSemaphoreFlag(ACCESS_TO_WAITING_CLIENTS, semid);
    printf("lockBarber\n");
    if(* WAITING_CLIENTS == 0) {
      printf("A\n");
      // A
      lockSemaphoreFlag(ACCESS_TO_BARBER_SLEEP, semid);
      barberSleep(timeBegin, semid);
      unlockSemaphoreFlag(ACCESS_TO_BARBER_SLEEP, semid);
      unlockSemaphoreFlag(ACCESS_TO_WAITING_CLIENTS, semid);
      // wait for client action
      lockSemaphoreFlag(SLEEP, semid);
      barberBeginShave(timeBegin, semid);
      barberEndShave(timeBegin, semid);
      // let client continue
      unlockSemaphoreFlag(FINISH, semid);
      // wait for client action
      lockSemaphoreFlag(LEAVE, semid);
    }
    else {
      printf("B\n");
      // B
      barberInvite(timeBegin, semid);
      unlockSemaphoreFlag(ACCESS_TO_WAITING_CLIENTS, semid);
      // let client continue
      unlockSemaphoreFlag(INVITE, semid);
      // wait for client action
      lockSemaphoreFlag(TAKEN_SEAT, semid);
      barberBeginShave(timeBegin, semid);
      barberEndShave(timeBegin, semid);
      // let client continue
      unlockSemaphoreFlag(FINISH, semid);
      // wait for client action
      lockSemaphoreFlag(LEAVE, semid);
    }
  }

  clearSemaphoresAndSharedMemory();
}

void clearSemaphoresAndSharedMemory() {
  /* Create queue */
  /* Variables */
  key_t key;
  int semid;
  int shmid;
  /* Set key generator seed */
  char * pathName = malloc(PATH_MAX * sizeof(char));
  pathName = getcwd(pathName, PATH_MAX);
  if((key = ftok(pathName, PROJ_NAME)) == -1)
  endWithError(-1);

  /* Clear section */
  /* Clear shared memory */
  shmdt(QUEUE);
  shmdt(CURR_SEAT);
  shmdt(timeBeginShM);
  shmdt(BARBER_SEAT);
  shmdt(WAITING_CLIENTS);
  shmdt(SEATS_SHM);
  shmdt(BARBER_SLEEP);
  shmid = shmget(key, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  shmid = shmget(key + 1, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  shmid = shmget(key + 2, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  shmid = shmget(key + 4, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  shmid = shmget(key + 5, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  shmid = shmget(key + 6, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  shmid = shmget(key + 7, 0, 0600);
  shmctl(shmid, IPC_RMID, NULL);
  /* Clear semaphores */
  semid = semget(key + 3, 0, 0);
  semctl(semid, key + 3, IPC_RMID, NULL);
}

void agentSIG_C(int signum) {
  clearSemaphoresAndSharedMemory();
  exit(0);
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
      printf("Barber.inviteClient(%d) : ", QUEUE[* CURR_SEAT].id);
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

int tryToLockSemaphoreFlag(semaphoreFlags flag, int semid) {
  int result;

  struct sembuf sb;
  sb.sem_num = flag;
  sb.sem_op = -1;
  sb.sem_flg = IPC_NOWAIT;

  result = semop(semid, &sb, 1);
  printf("%d\n", result);
  return result;
}

void barberSleep(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_SLEEP);
  * BARBER_SLEEP = true;
}

void barberWakeUp(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_WAKE_UP);
  * BARBER_SLEEP = false;
}

// uses WAITING_CLIENTS
void barberInvite(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_INVITE);
  * WAITING_CLIENTS = * WAITING_CLIENTS - 1;
  QUEUE[* CURR_SEAT].id = 0;
  * CURR_SEAT = (* CURR_SEAT + 1) % * SEATS_SHM;
}

void barberBeginShave(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_BEGIN_SHAVE);
}

void barberEndShave(struct timespec timeBegin, int semid) {
  printBarberCommunicat(timeBegin, OP_END_SHAVE);
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

void endWithTextError(char * txt, int n) {
  printf("Error: %s\n", txt);
  exit(n);
}
