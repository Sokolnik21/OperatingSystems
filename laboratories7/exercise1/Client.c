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

fifo * QUEUE;
int * BARBER_SEAT;
int * CURR_SEAT;
int * WAITING_CLIENTS;
int * SEATS_SHM;
int * BARBER_SLEEP;
int FLAG_leaveWithoutShave = false;

void printTime(struct timespec timeCurr, struct timespec timeBegin);
void printClientCommunicat(struct timespec timeBegin, typeOfClientOperation op);

void lockSemaphoreFlag(semaphoreFlags flag, int semid);
void unlockSemaphoreFlag(semaphoreFlags flag, int semid);
void waitForSemaphoreFlag(semaphoreFlags flag, int semid);
int tryToLockSemaphoreFlag(semaphoreFlags flag, int semid);

void clientWakeUpBarber(struct timespec timeBegin, int semid);
void clientTakeSeatToShave(struct timespec timeBegin, int semid);
void clientLeaveBarberAfterShave(struct timespec timeBegin, int semid);
void clientTakeSeatInQueue(struct timespec timeBegin, int semid);
void clientLeaveBarberWithoutShave(struct timespec timeBegin, int semid);
void clientClearSeatToShave();

void endWithError(int n);
void endWithTextError(char * txt, int n);

void DBG_printQueue(fifo * QUEUE) {
  printf("///////QUEUE///////\n");
  printf("| ");
  int i;
  for(i = 0; i < * SEATS_SHM; i++) {
      printf("%d |\n", QUEUE[i].id);
  }
  printf("///////////////////\n");
}

int main(int argc, char * argv[]) {
  if(argc != 3)
    endWithTextError("Wrong amount of arguments", -1);

  /* Variables */
  struct timespec timeBegin;
  struct timespec * timeBeginShM;
  int SEATS;
  int shmid;
  int semid;
  // int mpid = getpid();
  // int DBG_cpid;
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
  if((shmid = shmget(key + 5, 0, 0)) == -1)
    endWithError(-1);
  WAITING_CLIENTS = shmat(shmid, NULL, 0);
  if((shmid = shmget(key + 6, 0, 0)) == -1)
    endWithError(-1);
  SEATS_SHM = shmat(shmid, NULL, 0);
  if((shmid = shmget(key + 7, 0, 0)) == -1)
    endWithError(-1);
  BARBER_SLEEP = shmat(shmid, NULL, 0);

  /* Get semaphore */
  semid = semget(key + 3, 0, 0);

  while(--clients >= 0) {
    if(fork() == 0) {
      while(--barberQuan >= 0) {
        lockSemaphoreFlag(ACCESS_TO_BARBER_SLEEP, semid);
        if(* BARBER_SLEEP == true) {
          * BARBER_SLEEP = false;
          unlockSemaphoreFlag(ACCESS_TO_BARBER_SLEEP, semid);
          printf("A\n");
          // A
          clientTakeSeatToShave(timeBegin, semid);
          clientWakeUpBarber(timeBegin, semid);
          //let barber continue
          unlockSemaphoreFlag(SLEEP, semid);
          // wait for barber action
          lockSemaphoreFlag(FINISH, semid);
          clientClearSeatToShave();
          clientLeaveBarberAfterShave(timeBegin, semid);
          // let barber continue
          unlockSemaphoreFlag(LEAVE, semid);
        }
        else {
          printf("B\n");
          // B
          lockSemaphoreFlag(ACCESS_TO_WAITING_CLIENTS, semid);
          unlockSemaphoreFlag(ACCESS_TO_BARBER_SLEEP, semid);
          if(* WAITING_CLIENTS == 0) {
            printf("C\n");
            // C
            clientTakeSeatInQueue(timeBegin, semid);
            if(FLAG_leaveWithoutShave) {
              FLAG_leaveWithoutShave = false;
              continue;
            }
            unlockSemaphoreFlag(ACCESS_TO_WAITING_CLIENTS, semid);
            // wait for barber action
            lockSemaphoreFlag(INVITE, semid);
            clientTakeSeatToShave(timeBegin, semid);
            // let barber continue
            unlockSemaphoreFlag(TAKEN_SEAT, semid);
            // wait for barber action
            lockSemaphoreFlag(FINISH, semid);
            clientClearSeatToShave();
            clientLeaveBarberAfterShave(timeBegin, semid);
            // let barber continue
            unlockSemaphoreFlag(LEAVE, semid);
          }
          else {
            printf("D\n");
            // D
            clientTakeSeatInQueue(timeBegin, semid);
            if(FLAG_leaveWithoutShave) {
              FLAG_leaveWithoutShave = false;
              continue;
            }
            unlockSemaphoreFlag(ACCESS_TO_WAITING_CLIENTS, semid);
            // wait for barber action
            lockSemaphoreFlag(INVITE, semid);
            clientTakeSeatToShave(timeBegin, semid);
            // let barber continue
            unlockSemaphoreFlag(TAKEN_SEAT, semid);
            // wait for barber action
            lockSemaphoreFlag(FINISH, semid);
            clientClearSeatToShave();
            clientLeaveBarberAfterShave(timeBegin, semid);
            // let barber continue
            unlockSemaphoreFlag(LEAVE, semid);
          }
        }
      }
    }
  }
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

int tryToLockSemaphoreFlag(semaphoreFlags flag, int semid) {
  int result;

  struct sembuf sb;
  sb.sem_num = flag;
  sb.sem_op = -1;
  sb.sem_flg = IPC_NOWAIT;

  result = semop(semid, &sb, 1);
  printf("Semaphore %d\n", result);
  return result;
}

void clientWakeUpBarber(struct timespec timeBegin, int semid) {
  printClientCommunicat(timeBegin, OP_WAKE_UP_BARBER);
}

void clientTakeSeatToShave(struct timespec timeBegin, int semid) {
  printClientCommunicat(timeBegin, OP_TAKE_SEAT_TO_SHAVE);
  * BARBER_SEAT = getpid();
}

void clientLeaveBarberAfterShave(struct timespec timeBegin, int semid) {
  printClientCommunicat(timeBegin, OP_LEAVE_AFTER_SHAVE);
}

void clientTakeSeatInQueue(struct timespec timeBegin, int semid) {
  if(* WAITING_CLIENTS == * SEATS_SHM) {
    clientLeaveBarberWithoutShave(timeBegin, semid);
    unlockSemaphoreFlag(ACCESS_TO_WAITING_CLIENTS, semid);
    FLAG_leaveWithoutShave = true;
    // return;
  }
  else {
    printClientCommunicat(timeBegin, OP_TAKE_SEAT_IN_QUEUE);
    int index = (* CURR_SEAT + * WAITING_CLIENTS) % * SEATS_SHM;
    QUEUE[index].id = getpid();
    * WAITING_CLIENTS = * WAITING_CLIENTS + 1;
  }
}

void clientLeaveBarberWithoutShave(struct timespec timeBegin, int semid) {
  printClientCommunicat(timeBegin, OP_LEAVE_WITHOUT_SHAVE);
}

void clientClearSeatToShave() {
  * BARBER_SEAT = 0;
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

void endWithTextError(char * txt, int n) {
  printf("Error: %s\n", txt);
  exit(n);
}
