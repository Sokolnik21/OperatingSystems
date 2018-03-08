#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "LibraryToWorkOnTable.h"

typedef enum {
  DYNAMIC,
  STATIC
  }
allocationStrategy;

char * createBlock(int blockSize);

typedef enum {
  CREATE_TABLE,
  DELETE_TABLE,
  FIND_ELEMENT,
  ADD_BLOCK,
  DELETE_BLOCK
}
operationsStrategy;

int parseAllocationType(allocationStrategy * strategy, char * arg);
int parseOperationsStrategy(operationsStrategy ** listOfOperations, char * arg);

int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 5) {
    printf("Wrong amount of arguments\n");
    return -1;
  }

  /* Variables */
  int numberOfElements;
  int blockSize;
  allocationStrategy allocationType;
  char * listOfOperations;

  /* Parsing variables */
  numberOfElements = atoi(argv[1]);
  blockSize = atoi(argv[2]);
  if(parseAllocationType(&allocationType, argv[3]) == -1)
    return -2;
  listOfOperations = argv[4];

  /* Setting dependation for random number generator */
  srand(time(NULL));


  allocationStrategy x = allocationType;
  char * tmp;
  tmp = createBlock(blockSize);
  printf("%s\n", tmp);
  free(tmp);

  operationsStrategy * dupa;
  int variable = parseOperationsStrategy(&dupa, listOfOperations);
  printf("%d\n", variable);

  return 0;
}

char * createBlock(int blockSize) {
  /* Allocating space for string */
  char * result = calloc(blockSize, sizeof(char));

  /* Creating an instance of random number generator */
  int rng;

  /* Set cells with [a..z] chars */
  while (blockSize-- > 0) {
    rng = rand();
    result[blockSize] = (char) (rng % 26) + 97;
  }

  return result;
}

int parseAllocationType(allocationStrategy * strategy, char * arg) {
  switch(arg[0]) {
    case 'd': * strategy = DYNAMIC;
              return 0;
    case 's': * strategy = STATIC;
              return 0;
    default : printf("Wrong allocation type\n");
              printf("3. argument should be 's' - static or 'd' - dynamic\n");
              return -1;
  }
}

int parseOperationsStrategy(operationsStrategy ** listOfOperations, char * arg) {
  /* Setting size of the list */
  int i = 0;
  while (arg[i] != '\0') {
    printf("a\n");
    i++;
  }

  i = 0;
  while (arg[i] != '\0') {
    switch(arg[i]) {
      case 'c'  : break;
    }
    // printf("%c\n", arg[i]);
    i++;
  }
}
