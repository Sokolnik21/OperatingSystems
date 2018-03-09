#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "LibraryToWorkOnTable.h"

#define NUMBER_OF_OPERATIONS 3

typedef enum {
  DYNAMIC,
  STATIC
  }
allocationStrategy;

char * createBlock(int blockSize);

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
int parseOperationsStrategy(int blockSize, char * typeOfOperations, int * quantityOfOperations, char * arg);

/**
 * Main function
 */
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
  char typeOfOperations[NUMBER_OF_OPERATIONS];
  int quantityOfOperations[NUMBER_OF_OPERATIONS];

  /* Parsing variables */
  numberOfElements = atoi(argv[1]);
  blockSize = atoi(argv[2]);
  if(parseAllocationType(&allocationType, argv[3]) == -1)
    return -2;
  if(parseOperationsStrategy(blockSize, typeOfOperations, quantityOfOperations, argv[4]) == -1)
    return -3;

  /* Setting dependation for random number generator */
  srand(time(NULL));

  /* Variables usesd in main loop */
  char ** table;
  char * foundElement;
  int counter;

  /* Parser loop */
  int loops = 0;
  while(loops < 3) {
    switch(typeOfOperations[loops]) {
      case 'c'  : table = createTable(allocationType, numberOfElements);
                  break;
      case 'f'  : foundElement = findBlockWithSpecifiedQuantityInTable(allocationType, table, numberOfElements, quantityOfOperations[loops]);
                  printf("\nFound element: %s\n", foundElement);
                  break;
      case 'a'  : counter = quantityOfOperations[loops] - 1;
                  while(counter >= 0) {
                    addBlockToTable(allocationType, table, numberOfElements, createBlock(blockSize), counter);
                    counter--;
                  }
                  break;
      case 'd'  : counter = quantityOfOperations[loops] - 1;
                  while(counter >= 0) {
                    removeBlockFromTable(allocationType, table, numberOfElements, counter);
                    counter--;
                  }
                  break;
      case 'm'  : counter = quantityOfOperations[loops] - 1;
                  while(counter >= 0) {
                    addBlockToTable(allocationType, table, numberOfElements, createBlock(blockSize), counter);
                    counter--;
                  }
                  counter = quantityOfOperations[loops] - 1;
                  while(counter >= 0) {
                    removeBlockFromTable(allocationType, table, numberOfElements, counter);
                    counter--;
                  }
                  break;
      default : printf("Wrong operation type\n");
                printf("4. argument should be in format [c]:[i]:[c]:[i]:[c]:[i]\n");
                printf("Where c should be 'c', 'f', 'a', 'd' or 'm'\n");
                printf("And i should be an integer\n");
                return -3;
    }
    loops++;
  }

  /**
   * Test for parser
   */
  int iter;
  int innerIter;
  switch(allocationType) {
    case DYNAMIC:
      printf("\nDynamic Table:\n");
      for(iter = 0; iter < numberOfElements; iter++) {
        if(table[iter] != 0) {
          printf("%s\n", table[iter]);
        }
      }
      break;
    case STATIC:
      printf("\nStatic Table:\n");
      for(iter = 0; iter < numberOfElements; iter++) {
        if(staticTable[iter][0] != '\0') {
          printf("%s\n", staticTable[iter]);
        }
      }
      break;
  }

  /**
   * Generate data with
   */
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

int parseOperationsStrategy(int blockSize, char * typeOfOperations, int * quantityOfOperations, char * arg) {
  /* Setting variables */
  char op1, op2, op3;
  int qan1, qan2, qan3;
  sscanf (arg,"%c:%d:%c:%d:%c:%d",&op1,&qan1,&op2,&qan2,&op3,&qan3);

  /* Filling arrays */
  typeOfOperations[0] = op1;
  typeOfOperations[1] = op2;
  typeOfOperations[2] = op3;

  quantityOfOperations[0] = qan1;
  quantityOfOperations[1] = qan2;
  quantityOfOperations[2] = qan3;

  /* Check quantityOfOperations correctness */
  int i;
  for(i = 0; i < 3; i++) {
    if(blockSize < quantityOfOperations[i]) {
      if(typeOfOperations[i] == 'm' || typeOfOperations[i] == 'a' || typeOfOperations[i] == 'd') {
        printf("Too large amount of blocks to operate on in 4. argument\n");
        return -1;
      }
    }
  }
}
