#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

void endWithError(int n);
void endWithTextError(char * txt, int n);

int main(int argc, char * argv[]) {
  if(argc != 3)
    endWithError(-1);

  /* Variables */
  int side;
  int part;
  FILE * f;
  char * fileName;
  double ** filterTable;
  int i;

  /* Set rand dependation */
  srand(time(NULL));

  /* Initialize */
  side = atoi(argv[1]);
  part = atoi(argv[2]);
  fileName = "RAND.filter";
  filterTable = malloc(side * sizeof(double *));
  for(i = 0; i < side; i++) {
    filterTable[i] = malloc(side * sizeof(double));
  }

  /* Fill filtertable */
  /* Fill filterTable with zeros */
  int j;
  for(i = 0; i < side; i++)
    for(j = 0; j < side; j++)
      filterTable[i][j] = 0;
  /* Add concrete values */
  for(i = 0; i < part; i++) {
    int x = rand() % side;
    int y = rand() % side;
    double addition = (double)1/part;
    filterTable[x][y] = filterTable[x][y] + addition;
  }

    printf("%d\n", rand());

  /* Create file */
  f = fopen(fileName, "w");
  fprintf(f, "%d\n", side);
  for(i = 0; i < side; i++) {
    for(j = 0; j < side; j++)
      fprintf(f, "%f ", filterTable[i][j]);
    fprintf(f, "\n");
  }

  return 0;
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

void endWithTextError(char * txt, int n) {
  printf("Error: %s\n", txt);
  exit(n);
}
