/**
  A little summary of this exercise:
    "Signals sucks"
  Why?
    Almost every signal type is not queued.
    - It leads to inappropriate program behavior that not always occurs.
      The problem appears when program receives the same type of signal in the
      moment when this type is currently handling.
      When it happens then new signal alert is ignored.
       ~ That's why using SIGUSR1 to inform main program is bad idea.
    [The solution could be to use SIGRTMIN + N to store signals in queue
    but int this exercise it was forbidden]
    P.S.
    I've got the same problem with receiving CHLD signals. But in that case
    I was able to stagger children processes ending.
 */
/**
 Some interesting facts:
  sleep() function breaks when program receives signal.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Sleep
#include <unistd.h>

// Signal
#include <signal.h>

// Wait
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

/**
  Why global?
    Becouse signal modifies it
 */
int receivedRequests = 0;
int receivedRTsignals = 0;
int letContinue = FALSE;
int letEnd = FALSE;

// int childrenQuantity;
int childrenNumber;

/**
  Declare arrays
 */
/* To store children pid */
int * childrenPid;
/* To store children Real Time signal value */
int * childrenRTSignal;
/* To store information whether child was allowed to end */
int * childrenHandledEndSignal;
/* To store information about child return value */
int * childrenEndSignal;

int * createArray(int number) {
  int * result = malloc(number * (sizeof(int)));
  return result;
}

/**
  Signal handlers
 */
/* Basic handlers for main */
void handleSigint(int signum);
void handleSigusr1(int signum);
/* Work flow handlers for children */
void letMeContinue(int signum);
void letMeEnd(int signum);
/* Children detail handlers for main */
void mainCHLD(int signum, siginfo_t * si, void * context);
void mainRT(int signum, siginfo_t * si, void * context);

int main(int argc, char * argv[]) {
  /**
    Signal handlers
   */
  /* Handle SIGINT */
  signal(SIGINT, handleSigint);
  /* Handle SIGUSR1 */
  signal(SIGUSR1, handleSigusr1);
  /* Handle child end signal */
  struct sigaction childAct;
  childAct.sa_flags = SA_SIGINFO;
  childAct.sa_sigaction = mainCHLD;
  sigfillset(&childAct.sa_mask);
  sigdelset(&childAct.sa_mask, SIGCHLD);
  sigaction(SIGCHLD, &childAct, NULL);
  /* Handle SIGRT signals */
  struct sigaction rtAct;
  rtAct.sa_flags = SA_SIGINFO;
  rtAct.sa_sigaction = mainRT;
  sigemptyset(&rtAct.sa_mask);

  int iRT;
  for(iRT = 0; iRT < 32; iRT ++)
    sigaction(SIGRTMIN + iRT, &rtAct, NULL);

  /* Variables */
  int neededSignals;
  int mainPid = getpid();
  int tmpPid;

  /* Parse arguments */
  childrenNumber = atoi(argv[1]);
  neededSignals = atoi(argv[2]);

  /* Time dependation */
  srand(time(NULL));
  int rng;

  /**
    Create arrays
   */
  /* To store children pid */
  childrenPid = createArray(childrenNumber);
  /* To store children Real Time signal value */
  childrenRTSignal = createArray(childrenNumber);
  /* To store information whether child was allowed to end */
  childrenHandledEndSignal = createArray(childrenNumber);
  /* To store information about child return value */
  childrenEndSignal = createArray(childrenNumber);

  /* Filling childrenRTSignal */
  int iterRT;
  for(iterRT = 0; iterRT < childrenNumber; iterRT++)
    childrenRTSignal[iterRT] = -1;

  /* Filling childrenHandledEndSignal */
  int iterEnd;
  for(iterEnd = 0; iterEnd < childrenNumber; iterEnd++) {
    childrenHandledEndSignal[iterEnd] = FALSE;
  }

  /* Filling childrenPid array */
  int i;
  for(i = 0; i < childrenNumber; i++) {
    tmpPid = getpid();
    if(tmpPid == mainPid) {
      rng = rand() % 11;
      childrenPid[i] = fork();
      if(childrenPid[i] == 0) {
        signal(SIGUSR2, letMeContinue);
        signal(SIGINT, SIG_DFL);
        /* In child process */
        /* Here starts every child body */
        sleep(rng);
        kill(getppid(), SIGUSR1);
        while(!letContinue) {
          sleep(1);
        }
        int randSig = rand() % 32;
        kill(getppid(), SIGRTMIN + randSig);
          /* Handle SIGCHLD signals start */
          signal(SIGUSR2, letMeEnd);
          while(!letEnd) {
            sleep(1);
          }
          /* Handle SIGCHLD signals end */
        return rng;
        /* Here ends child body */
      }
    }
  }

  tmpPid = getpid();

  /* Value used to break loops in case when some signals are uncatched */
  /* Why 10 + childrenNumber?
      - Each iteration's duration needs 1 second to evaluate.
        It is said that every program can evaluate max 10 seconds.
        It can be break when signal is received.
        This break can occur once for every children.
        That's why it's 10 + childrenNumber
   */
  /* This is increadibly bad, not-intuitive and complicated solution for this problem.
      I think that better solution could be to use real time signals
      instead of SIGUSR1 to inform main program.
      Then signals could be stored in queue and the possibility of loosing
      signals would decrease to zero.
      However as I said in the beginning it is not allowed. - Deal with it.
   */
  int possibleError = 10 + childrenNumber;

  if(tmpPid == mainPid) {
    while(!letContinue) {
      if(receivedRequests >= neededSignals) {
        /* When requirement is met possibleError is set to find 3 empty iterations.
           I assume that when it happens then error could occure.
         */
        possibleError = 3;
        letContinue = TRUE;

        int i;
        for(i = 0; i < childrenNumber; i++) {
          kill(childrenPid[i], SIGUSR2);
        }
      }
      sleep(1);
      possibleError--;
      printf("Waiting\n");
      if(possibleError == 0) {
        printf("Some signals were uncatched\n");
        kill(getpid(), SIGINT);
      }
    }
    /* Handle SIGCHLD signals */
    while(TRUE) {
      int iter;
      for(iter = 0; iter < childrenNumber; iter++) {
        if(childrenHandledEndSignal[iter] == FALSE && childrenRTSignal[iter] != -1) {
          possibleError = 3;
          printf("End of the child with pid: %d\n", childrenPid[iter]);
          kill(childrenPid[iter], SIGUSR2);
          childrenHandledEndSignal[iter] = TRUE;
          sleep(1);
        }
      }
      sleep(1);
      possibleError--;
      /* Why do I compare childrenNumber with receivedRTsignals instead of
        receivedRequests?
          - There is huge possibility that some not real time signals can
            be uncatched. (It's bad idea to base on SIGUSR1 received signals)
            So to decrease it I use real time signals to find out whether
            child process executed properly or not.
       */
      if(receivedRTsignals == childrenNumber) {
        break;
      }
      if(possibleError == 0) {
        printf("Some signals were uncatched\n");
        kill(getpid(), SIGINT);
      }
    }
    int k;
    for(k = 0; k < childrenNumber; k++)
      printf("PID %d, CHLD end: %d\n", childrenPid[k], childrenEndSignal[k]);
    printf("------------------------\n");
    for(k = 0; k < childrenNumber; k++)
      printf("PID %d, Real Time signal: %d\n", childrenPid[k], childrenRTSignal[k]);
  }

  return 0;
}

void handleSigint(int signum) {
  int i;
  for(i = 0; i < childrenNumber; i++) {
    kill(childrenPid[i], SIGINT);
  }
  exit(0);
}

void handleSigusr1(int signum) {
  receivedRequests++;
}

void letMeContinue(int signum) {
  letContinue = TRUE;
}

void letMeEnd(int signum) {
  letEnd = TRUE;
}

void mainCHLD(int signum, siginfo_t * si, void * context) {
  int i = 0;
  while(childrenPid[i] != si -> si_pid) {
    i++;
  }
  childrenEndSignal[i] = si -> si_status;
}

void mainRT(int signum, siginfo_t * si, void * context) {
  int i = 0;
  while(childrenPid[i] != si -> si_pid) {
    i++;
  }
  childrenRTSignal[i] = (si -> si_signo) - SIGRTMIN;
  receivedRTsignals++;
}
