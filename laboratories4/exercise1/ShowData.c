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

/*
 Change:
    Instead of CTRL + Z this program uses CTRL + \
  Why?
    I was trying to implement CTRL + Z but my Op Sys after receives that signal
    stops program every single time.
    So to dispel my weekly misery I've made that little trick.
 */

/**
  Handling signals
 */
void agentSIG_C(int signum);
void agentSIG_Z(int signum);

void workingSIG_Z(int signum);

void mainUSR_1(int signum);
void mainUSR_2(int signum);
void mainTSTP(int signum);

int main(int argc, int * argv[]) {
  /* Setting masks for main process */
  sigset_t blockAllButUsrMask;
  sigfillset(&blockAllButUsrMask);
  sigdelset(&blockAllButUsrMask, SIGUSR1);
  sigdelset(&blockAllButUsrMask, SIGUSR2);
  sigdelset(&blockAllButUsrMask, SIGQUIT);

  sigset_t blockAllButIntTstp;
  sigfillset(&blockAllButIntTstp);
  sigdelset(&blockAllButIntTstp, SIGINT);
  sigdelset(&blockAllButIntTstp, SIGQUIT);

  signal(SIGUSR1, mainUSR_1);
  signal(SIGUSR2, mainUSR_2);
  signal(SIGQUIT, mainTSTP);

  while(TRUE) {
    pid_t mpid = fork();
    if(mpid != 0) {
      signal(SIGINT, SIG_DFL);
      signal(SIGQUIT, SIG_IGN);
      sigprocmask(SIG_SETMASK, &blockAllButUsrMask, NULL);
      pause();
      signal(SIGQUIT, mainTSTP);
      sigprocmask(SIG_SETMASK, &blockAllButIntTstp, NULL);
      pause();
    }
    if(mpid == 0) {
      /* Setting mask for agent */
      sigset_t unblockAllMask;
      sigemptyset(&unblockAllMask);
      sigprocmask(SIG_SETMASK, &unblockAllMask, NULL);

      /**
        Handling signal for agent
       */
      struct sigaction agentAct;
      /* Handle SIGQUIT [ctrl + \] */
      agentAct.sa_handler = agentSIG_Z;
      sigemptyset(&agentAct.sa_mask);
      agentAct.sa_flags = 0;
      sigaction(SIGQUIT, &agentAct, NULL);
      /* Handle SIGINT [ctrl + c] */
      signal(SIGINT, agentSIG_C);

      while(TRUE) {
        int innerStatus;
        pid_t cpid = fork();
        waitpid(cpid, &innerStatus, 0);
        if(cpid == 0) {
          /* Setting default signal handler for inner process */
          signal(SIGINT, SIG_DFL);
          signal(SIGQUIT, workingSIG_Z);

          sleep(1);
          execlp("date", "date", NULL);
        }
      }
    }
  }

  return 0;
}

void agentSIG_C(int signum) {
  kill(getppid(), SIGUSR1);
  kill(getpid(), SIGKILL);
}

void agentSIG_Z(int signum) {
  printf("agent \\\n");
  kill(getppid(), SIGUSR2);
  kill(getpid(), SIGKILL);
}

void workingSIG_Z(int signum) {
  kill(getppid(), SIGQUIT);
  kill(getpid(), SIGKILL);
}

void mainUSR_1(int signum) {
  printf("\nReceived SIGINT signal\n");
  kill(getpid(), SIGKILL);
}

void mainUSR_2(int signum) {
  printf("\nWaiting for signal\n");
  printf("CTRL + \\ to continue\n");
  printf("CTRL + C to end\n");
}

void mainTSTP(int signum) {
  // Do nothing
}
