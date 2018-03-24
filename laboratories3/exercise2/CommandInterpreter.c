#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

// Processes
#include <sys/wait.h>

// Split string
#include <string.h>

#define MAX_ARGUMENTS 10

int main(int argc, char * argv[]) {
  /* Setting file handler */
  FILE * f = fopen("Commands.txt", "r");
  if (f == NULL)
  {
      printf("Error opening file!\n");
      exit(1);
  }

  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, f)) != -1) {
    char ** operations = malloc(MAX_ARGUMENTS * sizeof(char *));
    int i = 0;

    /* Split string */
    // Create copy
    char * tmpLine;
    strcpy(tmpLine, line);
    // Splitting tmpLine at every " " or "\n"
    char * word;
    word = strtok(tmpLine," \n");
    while(word != NULL) {
      operations[i++] = word;
      word = strtok (NULL, " \n");
    }

    int status;
    pid_t cpid = fork();
    waitpid(cpid, &status, WUNTRACED | WCONTINUED);
    if(cpid == 0) {
      execlp(operations[0], operations[0], operations[1], operations[2], (char * ) NULL);
      // Error handler
      printf("alaska\n");
      _exit(status);
      //
    }
    int j;
    for(j = 0 ;j < MAX_ARGUMENTS; j++)
      if(operations[j] != NULL)
        operations[j] = NULL;
      else
        break;
  }

  fclose(f);

  return 0;
}
