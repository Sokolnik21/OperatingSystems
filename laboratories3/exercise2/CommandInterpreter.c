#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// Processes
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

// Split string
#include <string.h>

#define MAX_INNER_PROG_ARGS 10

/**
  Main
 */
int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 2) {
    printf("Wrong amount of arguments\n");
    return -1;
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

  /* Parsing variables */
  strcpy(filePath, argv[1]);

  /* Setting file handler */
  FILE * f = fopen(filePath, "r");
  if (f == NULL) {
      printf("Error opening file!\n");
      exit(1);
  }

  /* Get ever line from file and */
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

    int i = 0;
    while(word != NULL) {
      innerProgArgs[i] = malloc(sizeof(word));
      strcpy(innerProgArgs[i++], word);
      word = strtok(NULL, "\n ");
    }

    int status;
    pid_t cpid = fork();
    waitpid(cpid, &status, WUNTRACED | WCONTINUED);
    if(cpid == 0) {
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

    // Clear innerProgArgs table
    int j;
    for(j = 0 ;j < MAX_INNER_PROG_ARGS; j++)
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
