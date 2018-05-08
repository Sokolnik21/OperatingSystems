#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
// Split string
#include <string.h>
// PATH_MAX
#include <limits.h>
#include <time.h>

int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 2) {
    printf("Wrong amount of arguments\n");
    return -1;
  }

  /* Variables */
  char filePath[PATH_MAX];
  char * line = malloc(PATH_MAX * sizeof(char));
  size_t len = 0;
  ssize_t readLine;

  /* Parsing variables */
  strcpy(filePath, argv[1]);

  /* Create named pipe */
  mkfifo(filePath, 0600);

  /* Open named pipe */
  /* Setting file handler */
  FILE * f = fopen(filePath, "r");
  if (f == NULL) {
      printf("Error opening file!\n");
      exit(1);
  }

  /* Read from named pipe */
  while(1) {
    while ((readLine = getline(&line, &len, f)) != -1) {
      printf("%s", line);
    }
    sleep(1);
  }

  /* Close named pipe */
  fclose(f);

  return 0;
}
