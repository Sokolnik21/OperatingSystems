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

#define MAX_BUF 100

int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 3) {
    printf("Wrong amount of arguments\n");
    return -1;
  }

  /* Variables */
  char filePath[PATH_MAX];
  int actionQuantity;
  FILE * f;
  FILE * d;
  char * date = malloc(MAX_BUF * sizeof(char));

  /* Parsing variables */
  strcpy(filePath, argv[1]);
  actionQuantity = atoi(argv[2]);

  /* Set time dependency */
  srand(time(NULL));

  printf("%d\n", getpid());

  /* Read from named pipe */
  int currAction;
  for(currAction = 0; currAction < actionQuantity; currAction++) {
    f = fopen(filePath, "w");
    if (f == NULL) {
      printf("Error opening file!\n");
      exit(1);
    }
    d = popen("date", "r");

    fread(date, 1, 100, d);
    fprintf(f, "%d: %s", getpid(), date);
    sleep(rand() % 4 + 2);
    pclose(d);
    fclose(f);
  }

  return 0;
}
