#include <stdio.h>
#include <time.h>
#include <stdlib.h>
// Sleep
#include <unistd.h>
// Signal
#include <signal.h>
// Wait
#include <sys/wait.h>
// Shared memory
#include <sys/mman.h>

#define TRUE 1
#define FALSE 0

/* Global variables */
int letContinue = TRUE;
int mainPid;
int mainReceivedSigusr = 0;
static int * chldPid;
static int * chldReceivedSigusr;

int * createArray(int number) {
  int * result = malloc(number * (sizeof(int)));
  return result;
}

/**
  Signal handlers
 */
/* Handlers for main */
void mainSigusr1(int signum);
/* Handlers for child */
void chldSigusr1(int signum);
void chldSigusr2(int signum);

int main(int argc, char * argv[]) {
  /**
    Signal handlers
   */
  /* Handle main SIGUSR */
  signal(SIGUSR1, mainSigusr1);

  /* Variables */
  int signalQuantity;
  int operationType;
  mainPid = getpid();
  chldPid = mmap(NULL, sizeof * chldPid, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  chldReceivedSigusr = mmap(NULL, sizeof * chldReceivedSigusr, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  * chldReceivedSigusr = 0;

  /* Parse arguments */
  signalQuantity = atoi(argv[1]);
  operationType = atoi(argv[2]);

  /* Create child */
  fork();

  /* Child pid */
  if(getpid() != mainPid) {
    * chldPid = getpid();
    /* Handle chld SIGUSR */
    signal(SIGUSR1, chldSigusr1);
    signal(SIGUSR2, chldSigusr2);

    while(TRUE)
      sleep(1);
  }

  /* Parent pid */
  if(getpid() == mainPid) {
    while(* chldPid == 0)
      sleep(1);
    int iterSignal;
    switch(operationType) {
      case 1 :
        for(iterSignal = 0; iterSignal < signalQuantity; iterSignal++) {
          kill(* chldPid, SIGUSR1);
        }
        kill(* chldPid, SIGUSR2);
        break;
      case 2 :
        for(iterSignal = 0; iterSignal < signalQuantity; iterSignal++) {
          while(!letContinue)
            sleep(1);
          kill(* chldPid, SIGUSR1);
          letContinue = FALSE;
        }
        while(!letContinue)
          sleep(1);
        kill(* chldPid, SIGUSR2);
        break;
    }

    /* Print values */
    printf("Sent messages: %d\n", signalQuantity);
    printf("Children received signals: %d\n", * chldReceivedSigusr);
    printf("Main received signals: %d\n", mainReceivedSigusr);
  }

  return 0;
}

/**
  Signal handlers
 */
/* Handlers for main */
void mainSigusr1(int signum) {
  letContinue = TRUE;
  mainReceivedSigusr++;
}
/* Handlers for child */
void chldSigusr1(int signum) {
  * chldReceivedSigusr = * chldReceivedSigusr + 1;
  kill(mainPid, SIGUSR1);
}
void chldSigusr2(int signum) {
  kill(* chldPid, SIGKILL);
}
