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

// PATH_MAX
#include <limits.h>

#include <time.h>

#define MAX_WORDS_IN_COMMAND 10
#define MAX_NUMBER_OF_COMMANDS_IN_LINE 10
#define MAX_PREVIOUS_OUTPUT 500

const char * EOA = "END_OF_ARGUMENTS";

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
  char * lineWords[MAX_WORDS_IN_COMMAND * MAX_NUMBER_OF_COMMANDS_IN_LINE];
  char ** lineCommands[MAX_NUMBER_OF_COMMANDS_IN_LINE];
  int iterLC;
  for(iterLC = 0; iterLC < MAX_NUMBER_OF_COMMANDS_IN_LINE; iterLC++) {
    lineCommands[iterLC] = malloc(MAX_WORDS_IN_COMMAND * sizeof(char * ));
  }
  // To pass into exec function
  char * execArgs[MAX_WORDS_IN_COMMAND];
  // To work with file
  char * line = malloc(PATH_MAX * sizeof(char));
  size_t len = 0;
  ssize_t readLine;
  // Pipe buffer
  char * previousOut = malloc(MAX_PREVIOUS_OUTPUT * sizeof(char));

  /* Parsing variables */
  strcpy(filePath, argv[1]);

  /* Setting file handler */
  FILE * f = fopen(filePath, "r");
  if (f == NULL) {
      printf("Error opening file!\n");
      exit(1);
  }

  /* Get every line from file and parse it */
  while ((readLine = getline(&line, &len, f)) != -1) {
    if(line[0] == '\n' || line[0] == ' ' || line[0] == '\t')
      continue;
    char * tmpLine = malloc(sizeof(line));
    strcpy(tmpLine, line);
    /* Split string at every " " or "\n" */
    char * word;
    word = strtok(line,"\n ");

    int i = 0;
    while(word != NULL) {
      lineWords[i] = malloc(sizeof(word));
      strcpy(lineWords[i++], word);
      word = strtok(NULL, "\n ");
    }

    // In lineWrods is curently the whole line from .txt file
    // Now I split lineWords whenever "|" word occures (pipe)
    int lineCommandNumber = 0;
    int lineCommandArgs = 0;

    int iterWord;
    for(iterWord = 0; lineWords[iterWord] != NULL; iterWord++) {
      if(strcmp(lineWords[iterWord], "|") != 0) {
        lineCommands[lineCommandNumber][lineCommandArgs] = malloc(sizeof(lineWords[iterWord]));
        strcpy(lineCommands[lineCommandNumber][lineCommandArgs], lineWords[iterWord]);
        lineCommandArgs++;
      }
      if(strcmp(lineWords[iterWord], "|") == 0) {
        /* Marker to know when args ends */
        lineCommands[lineCommandNumber][lineCommandArgs] = malloc(sizeof(EOA));
        strcpy(lineCommands[lineCommandNumber][lineCommandArgs], EOA);

        lineCommandNumber++;
        lineCommandArgs = 0;
      }
    }
    /* Marker to know when args ends for the last command */
    lineCommands[lineCommandNumber][lineCommandArgs] = malloc(sizeof(EOA));
    strcpy(lineCommands[lineCommandNumber][lineCommandArgs], EOA);
    /* Markser to know when commands end */
    lineCommands[lineCommandNumber + 1][0] = malloc(sizeof(EOA));
    strcpy(lineCommands[lineCommandNumber + 1][0], EOA);

    // Working start
    int x, y;
    for(x = 0; strcmp(lineCommands[x][0], EOA) != 0; x++) {
      /* Reset execArgs table */
      for(iterLC = 0; iterLC < MAX_WORDS_IN_COMMAND; iterLC++) {
        execArgs[iterLC] = NULL;
      }
      /* Pass command arguments to execArgs */
      for(y = 0; strcmp(lineCommands[x][y], EOA) != 0; y++) {
        execArgs[y] = malloc(sizeof(lineCommands[x][y]));
        strcpy(execArgs[y], lineCommands[x][y]);
      }
      /* Set last argument in execArgs to NULL */
      execArgs[y] = NULL;

      /* Exec function start */
      int status;
      int fdChldIn[2];
      int fdChldOut[2];
      pipe(fdChldIn);
      pipe(fdChldOut);
      pid_t cpid = fork();
      /* Child */
      if(cpid == 0) {
        /* fdChldIn - update standard input */
        close(fdChldIn[1]);
        dup2(fdChldIn[0], STDIN_FILENO);
        close(fdChldIn[0]);

        /* fdChldOut - update standard output */
        close(fdChldOut[0]);
        if(strcmp(lineCommands[x + 1][0], EOA) == 0) {
          dup2(STDOUT_FILENO, fdChldOut[1]);
          close(fdChldOut[1]);
        }
        else {
          dup2(fdChldOut[1], STDOUT_FILENO);
          close(fdChldOut[1]);
        }
      }
      /* Parent */
      else {
        /* fdChldIn */
        close(fdChldIn[0]);
        write(fdChldIn[1], previousOut, MAX_PREVIOUS_OUTPUT);
        close(fdChldIn[1]);
        /* fdChldOut */
        close(fdChldOut[1]);
        int iterNew;
        for(iterNew = 0; iterNew < MAX_PREVIOUS_OUTPUT; iterNew++)
          previousOut[iterNew] = 0;
        read(fdChldOut[0], previousOut, MAX_PREVIOUS_OUTPUT);
        close(fdChldOut[0]);
      }
      if(cpid != 0)
        waitpid(cpid, &status, WUNTRACED | WCONTINUED);
      if(cpid == 0) {
        execvp(execArgs[0], execArgs);
        // Error handler - if above instruction returns with error
        // then instructions below will be executed
        printf("\nError in execlp() function\n");
        printf("Incorrect line: %s\n", tmpLine);
        return -1;
      }
      // Error handler part 2 - if above function execvp() returns Error
      // then variable "status" won't has value = 0
      if(status != 0)
        return -1;
      /* Exec function end */
    }

    // Clear lineWords table
    int j;
    for(j = 0; j < MAX_WORDS_IN_COMMAND; j++)
      if(lineWords[j] != NULL) {
        free(lineWords[j]);
        lineWords[j] = NULL;
      }
      else
        break;

    // Clear lineCommands table
    for(x = 0; strcmp(lineCommands[x][0], EOA) != 0; x++) {
      for(y = 0; strcmp(lineCommands[x][y], EOA) != 0; y++) {
        free(lineCommands[x][y]);
      }
    }
  }

  // Close file
  fclose(f);

  return 0;
}
