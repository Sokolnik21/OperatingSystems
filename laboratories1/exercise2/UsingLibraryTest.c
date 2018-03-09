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
  ADD_BLOCK,
  DELETE_BLOCK,
  FIND_ELEMENT
}
operationsStrategy;

/**
 * Operation standarise
 */
char ** createTable(allocationStrategy strategy, int cellsQuantity);
void deleteTable(allocationStrategy strategy, char ** table, int cellsQuantity);

int addBlockToTable(allocationStrategy strategy, char ** table,
  int cellsQuantity, char * block, int cell);
int removeBlockFromTable(allocationStrategy strategy, char ** table,
  int cellsQuantity, int cell);

char * findBlockWithSpecifiedQuantityInTable(allocationStrategy strategy,
  char ** table, int cellsQuantity, int quantity);

/**
 * Parsing functions
 */
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


  // allocationStrategy x = allocationType;
  // char * tmp;
  // tmp = createBlock(blockSize);
  // printf("%s\n", tmp);
  // free(tmp);
  //
  // char ** table = createTable(allocationType, numberOfElements);
  //
  // operationsStrategy * dupa;
  // int variable = parseOperationsStrategy(&dupa, listOfOperations);
  // printf("%d\n", variable);

  return 0;
}

// int main() {
//   /* Dynamic test */
//   char ** a = createTable(allocationType, 5);
//   addBlockToTable(allocationType, a, 5, createBlock(blockSize), 0);
//   addBlockToTable(allocationType, a, 5, "Jagalat", 0);
//   removeBlockFromTable(allocationType, a, 5, 0);
//   char * b = findBlockWithSpecifiedQuantityInTable(allocationType, a, 5, 7);
//   // printf("%s\n", a[0]);
//   printf("%s\n", b);
//
//   /* Static test */
//   initializeStaticTable();
//   addBlockToStaticTable("Jaganala", 0);
//   int stringSize = getStringSize(staticTable[0]);
//   printf("%d\n", stringSize);
//   char * w = findBlockWithSpecifiedQuantityInStaticTable(7);
//   printf("w: %s\n", w);
//   return 0;
// }

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

/**
 * Operation standarise
 */
char ** createTable(allocationStrategy strategy, int cellsQuantity) {
  switch (strategy) {
    case STATIC:  initializeStaticTable();
                  return NULL;
    case DYNAMIC: return createDynamicTable(cellsQuantity);
  }
}

void deleteTable(allocationStrategy strategy, char ** table, int cellsQuantity) {
  switch (strategy) {
    case STATIC:  initializeStaticTable();
    case DYNAMIC: deleteDynamicTable(table, cellsQuantity);
  }
}

int addBlockToTable(allocationStrategy strategy, char ** table,
  int cellsQuantity, char * block, int cell) {

  switch (strategy) {
    case STATIC:  return addBlockToStaticTable(block, cell);
    case DYNAMIC: return addBlockToDynamicTable(table, cellsQuantity, block, cell);
  }
}

int removeBlockFromTable(allocationStrategy strategy, char ** table,
  int cellsQuantity, int cell) {

  switch (strategy) {
    case STATIC:  return removeBlockFromStaticTable(cell);
    case DYNAMIC: return removeBlockFromDynamicTable(table, cellsQuantity, cell);
  }
}

char * findBlockWithSpecifiedQuantityInTable(allocationStrategy strategy,
  char ** table, int cellsQuantity, int quantity) {

  switch (strategy) {
    case STATIC:  return findBlockWithSpecifiedQuantityInStaticTable(quantity);
    case DYNAMIC: return findBlockWithSpecifiedQuantityInDynamicTable(table, cellsQuantity, quantity);
  }
}

/**
 * Parsing functions
 */
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
  /* Allocating space for operations queue */
  int i = 0;
  while (arg[i] != '\0') {
    printf("a\n");
    i++;
  }

  /* Filling queue with operations */
  i = 0;
  while (arg[i] != '\0') {
    switch(arg[i]) {
      case 'c'  : break;
    }
    // printf("%c\n", arg[i]);
    i++;
  }
}
