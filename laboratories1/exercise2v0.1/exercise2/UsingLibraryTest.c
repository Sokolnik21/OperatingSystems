#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "LibraryToWorkOnTable.h"

typedef enum {
  RANDOMCHAR,
  INCREASEINT,
  ONLYX
  }
blockStrategy;

char * createBlock(blockStrategy x, int blockSize);
char * createBlockRANDOMCHAR(int blockSize);
char * createBlockINCREASEINT(int blockSize);
char * createBlockONLYX(int blockSize);

typedef enum {
  CREATETABLE,
  DELETETABLE,
  FINDELEMENT,
  ADDBLOCK,
  DELETEBLOCK
}
operationsStrategy;

int parseAllocationType(blockStrategy * strategy, char * arg);

int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 5) {
    printf("Wrong amount of arguments\n");
    return -1;
  }

  /* Variables */
  int numberOfElements;
  int blockSize;
  blockStrategy allocationType;
  char * listOfOperations;

  /* Parsing variables */
  numberOfElements = atoi(argv[1]);
  blockSize = atoi(argv[2]);
  if(parseAllocationType(&allocationType, argv[3]) == -1)
    return -2;
  listOfOperations = argv[4];

  /* Setting dependation for random number generator */
  srand(time(NULL));


  blockStrategy x = allocationType;
  char * tmp;
  tmp = createBlock(x, blockSize);
  printf("%s\n", tmp);
  free(tmp);

  return 0;
}

char * createBlock(blockStrategy x, int blockSize) {
  switch(x) {
    case RANDOMCHAR:  return createBlockRANDOMCHAR(blockSize);
    case INCREASEINT: return createBlockINCREASEINT(blockSize);
    case ONLYX:       return createBlockONLYX(blockSize);
  }
}

char * createBlockRANDOMCHAR(int blockSize) {
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

char * createBlockINCREASEINT(int blockSize) {
  /* Allocating space for string */
  char * result = calloc(blockSize, sizeof(char));

  /* Variables*/
  int i = 0;

  /* Set cells with '[0..9] chars */
  while(i < blockSize) {
    result[i] = (char) i % 10 + 48;
    i++;
  }

  return result;
}

char * createBlockONLYX(int blockSize) {
  /* Allocating space for string */
  char * result = calloc(blockSize, sizeof(char));

  /* Set cells with 'X' char */
  while(blockSize-- > 0)
    result[blockSize] = 'X';

  return result;
}

int parseAllocationType(blockStrategy * strategy, char * arg) {
  switch(arg[0]) {
    case 'r': * strategy = RANDOMCHAR;
              return 0;
    case 'i': * strategy = INCREASEINT;
              return 0;
    case 'x': * strategy = ONLYX;
              return 0;
    default : printf("Wrong allocation type\n");
              printf("3. argument should be 'r', 'i' or 'x'\n");
              return -1;
  }
}
