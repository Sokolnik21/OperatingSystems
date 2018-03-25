#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// Processes
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

// limits
#include <sys/resource.h>

// Split string
#include <string.h>

// time
#include <sys/time.h>
// #include <time.h>

#define MAX_INNER_PROG_ARGS 10
#define MILLION 1E6

/**
  Main
 */
int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 4) {
    printf("Wrong amount of arguments\n");
    return -2;
  }

  /* Variables */
  // File path
  char filePath[PATH_MAX];
  // To store inner program arguments
  char * innerProgArgs[MAX_INNER_PROG_ARGS];
  // To work with file
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  // To handle limits
  struct rlimit cpuLimit;
  struct rlimit memLimit;

  /* Parsing variables */
  strcpy(filePath, argv[1]);
  // Limits
  cpuLimit.rlim_cur = atoi(argv[2]);
  cpuLimit.rlim_max = atoi(argv[2]);
  memLimit.rlim_cur = MILLION * atoi(argv[3]);
  memLimit.rlim_max = MILLION * atoi(argv[3]);

  /* Setting file handler to file with commands */
  FILE * f = fopen(filePath, "r");
  if (f == NULL) {
      printf("Error opening file!\n");
      exit(1);
  }

  /* Creating file Reports.txt */
  FILE * fRep = fopen("Reports.txt", "w");
  if (fRep == NULL) {
      printf("Error opening file!\n");
      exit(1);
  }
  fclose(fRep);

  /* Get every line from file and parse them into table */
  while ((read = getline(&line, &len, f)) != -1) {
    if(line[0] == '\n' || line[0] == ' ' || line[0] == '\t')
      continue;
    /* Split string */
    // Create copy
    char * tmpLine;
    tmpLine = malloc(sizeof(line));
    strcpy(tmpLine, line);
    // Splitting tmpLine at every " " or "\n"
    char * word;
    word = strtok(tmpLine,"\n ");

    // Filling table that stores command arguments
    int i = 0;
    while(word != NULL) {
      innerProgArgs[i] = malloc(sizeof(word));
      strcpy(innerProgArgs[i++], word);
      word = strtok(NULL, "\n ");
    }

    /**
      Here is a little trick
      I'm gonna use fork() function twice to get proper memory usage
      of the children process.
      Q&A:
        Why twice?
        - Field ru_maxrss from struct rusage shows only the value
          of the one child process that was largest one.
          To prevent this I fork main process and work on the child.
          Thanks to that every next child process have different parent
          and executing getrusage(2) function provides independent results.
     */
    int originStatus;
    pid_t originCpid = fork();
    waitpid(originCpid, &originStatus, WUNTRACED | WCONTINUED);
    if(originCpid == 0) {
      int status;
      pid_t cpid = fork();
      waitpid(cpid, &status, WUNTRACED | WCONTINUED);
      if(cpid == 0) {
        // Setting limits for child process
        setrlimit(RLIMIT_CPU, &cpuLimit);
        setrlimit(RLIMIT_AS, &memLimit);
        // Executing line from file
        execvp(innerProgArgs[0], innerProgArgs);
        // Error handler - if above instruction returns with error
        // then instructions below will be executed
        printf("\nError in execlp() function\n");
        printf("Incorrect line: %s\n", line);
        return -1;
      }
      // Error handler part 2 - if above function execvp() returns Error
      // then variable "status" won't has value = 0
      if(status != 0)
        return -1;

      /* Reports - begin */
      /* Setting file handler to file for reports */
      FILE * fRep = fopen("Reports.txt", "a");
      if (fRep == NULL) {
          printf("Error opening file!\n");
          exit(1);
      }

      // Title - describtion of the command
      fprintf(fRep, "%s", line);

      // Variables
      struct timeval userTime, sysTime;
      long memory;

      // Operations with rusage
      struct rusage usage;
      getrusage(RUSAGE_CHILDREN, &usage);
      memory = usage.ru_maxrss;
      userTime = usage.ru_utime;
      sysTime = usage.ru_stime;
      fprintf(fRep, "\tMEM: %ld kB\n", memory);
      fprintf(fRep, "\tUSER-time: %ld sec and %ld microsec\n", userTime.tv_sec, userTime.tv_usec);
      fprintf(fRep, "\tSYS-time:  %ld sec and %ld microsec\n", sysTime.tv_sec, sysTime.tv_usec);
      fprintf(fRep, "\n");

      fclose(fRep);
      /* Reports - end */
      _exit(0);
    }
    if(originStatus != 0)
      exit(-1);

    // Clear innerProgArgs table
    int j;
    for(j = 0; j < MAX_INNER_PROG_ARGS; j++)
      if(innerProgArgs[j] != NULL) {
        free(innerProgArgs[j]);
        innerProgArgs[j] = NULL;
      }
      else
        break;

    // Clear tmpLine
    free(tmpLine);
  }

  // Close file
  fclose(f);

  return 0;
}
