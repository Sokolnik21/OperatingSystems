#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "LibraryToWorkOnTable.h"

#define NUMBER_OF_OPERATIONS 3
#define BILLION 1E9

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
int parseOperationsStrategy(int numberOfElements, char * typeOfOperations, int * quantityOfOperations, char * arg);

/**
 * Testing functions
 */
double getTimeCreateTable(clockid_t clk_id, int numberOfElementsInTest, int blockSizeInTest);
double getTimeFindElement(clockid_t clk_id, char ** testTable, int numberOfElementsInTest);
double getTimeDeleteHalfAddHalf(clockid_t clk_id, char ** testTable, int numberOfElementsInTest, int blockSizeInTest);
double getTimeReplaceHalf(clockid_t clk_id, char ** testTable, int numberOfElementsInTest, int blockSizeInTest);

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
  if(parseOperationsStrategy(numberOfElements, typeOfOperations, quantityOfOperations, argv[4]) == -1)
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
   * Generate data with execution time
   */

  /* Setting file handler */
  FILE *f = fopen("raport2.txt", "w");
  if (f == NULL)
  {
      printf("Error opening file!\n");
      exit(1);
  }

  /* Variables */
  int numberOfElementsInTest = 10000;
  int blockSizeInTest = 3000;
  char ** testTable;
  struct timespec timeStart, timeEnd;
  long long seconds;
  long long nanoseconds;
  double timeDifference;
  int i;  // Iterator used to iterate table

  printf("\nExecution times for dynamic table with parameters:\n");
  fprintf(f, "Execution times for dynamic table with parameters:\n");
  printf("numberOfBlocks - %d, blockSize - %d\n\n", numberOfElementsInTest, blockSizeInTest);
  fprintf(f, "numberOfBlocks - %d, blockSize - %d\n\n", numberOfElementsInTest, blockSizeInTest);

  printf("Create table and fill it with data:\n");
  fprintf(f, "Create table and fill it with data:\n");
  /* REAL TIME */
  timeDifference = getTimeCreateTable(CLOCK_REALTIME, numberOfElementsInTest, blockSizeInTest);
  printf("\tREAL: %f sec\n", timeDifference);
  fprintf(f, "\tREAL: %f sec\n", timeDifference);
  /* USER TIME */
  timeDifference = getTimeCreateTable(CLOCK_MONOTONIC, numberOfElementsInTest, blockSizeInTest);
  printf("\tUSER: %f sec\n", timeDifference);
  fprintf(f, "\tUSER: %f sec\n", timeDifference);
  /* SYSTEM TIME */
  timeDifference = getTimeCreateTable(CLOCK_PROCESS_CPUTIME_ID, numberOfElementsInTest, blockSizeInTest);
  printf("\tSYST: %f sec\n", timeDifference);
  fprintf(f, "\tSYST: %f sec\n", timeDifference);

  /* Setting table */
  testTable = createDynamicTable(numberOfElementsInTest);
  for(i = 0; i < numberOfElementsInTest; i++)
    addBlockToDynamicTable(testTable, numberOfElementsInTest, createBlock(blockSizeInTest), i);

  printf("Find element:\n");
  fprintf(f, "Find element:\n");
  /* REAL TIME */
  timeDifference = getTimeFindElement(CLOCK_REALTIME, testTable, numberOfElementsInTest);
  printf("\tREAL: %f sec\n", timeDifference);
  fprintf(f, "\tREAL: %f sec\n", timeDifference);
  /* USER TIME */
  timeDifference = getTimeFindElement(CLOCK_MONOTONIC, testTable, numberOfElementsInTest);
  printf("\tUSER: %f sec\n", timeDifference);
  fprintf(f, "\tUSER: %f sec\n", timeDifference);
  /* SYSTEM TIME */
  timeDifference = getTimeFindElement(CLOCK_PROCESS_CPUTIME_ID, testTable, numberOfElementsInTest);
  printf("\tSYST: %f sec\n", timeDifference);
  fprintf(f, "\tSYST: %f sec\n", timeDifference);

  printf("Delete half blocks and then add half blocks:\n");
  fprintf(f, "Delete half blocks and then add half blocks:\n");
  /* REAL TIME */
  timeDifference = getTimeDeleteHalfAddHalf(CLOCK_REALTIME, testTable, numberOfElementsInTest, blockSizeInTest);
  printf("\tREAL: %f sec\n", timeDifference);
  fprintf(f, "\tREAL: %f sec\n", timeDifference);
  /* USER TIME */
  timeDifference = getTimeDeleteHalfAddHalf(CLOCK_MONOTONIC, testTable, numberOfElementsInTest, blockSizeInTest);
  printf("\tUSER: %f sec\n", timeDifference);
  fprintf(f, "\tUSER: %f sec\n", timeDifference);
  /* SYSTEM TIME */
  timeDifference = getTimeDeleteHalfAddHalf(CLOCK_PROCESS_CPUTIME_ID, testTable, numberOfElementsInTest, blockSizeInTest);
  printf("\tSYST: %f sec\n", timeDifference);
  fprintf(f, "\tSYST: %f sec\n", timeDifference);

  printf("Replace half blocks\n");
  fprintf(f, "Replace half blocks\n");
  /* REAL TIME */
  timeDifference = getTimeReplaceHalf(CLOCK_REALTIME, testTable, numberOfElementsInTest, blockSizeInTest);
  printf("\tREAL: %f sec\n", timeDifference);
  fprintf(f, "\tREAL: %f sec\n", timeDifference);
  /* USER TIME */
  timeDifference = getTimeReplaceHalf(CLOCK_MONOTONIC, testTable, numberOfElementsInTest, blockSizeInTest);
  printf("\tUSER: %f sec\n", timeDifference);
  fprintf(f, "\tUSER: %f sec\n", timeDifference);
  /* SYSTEM TIME */
  timeDifference = getTimeReplaceHalf(CLOCK_PROCESS_CPUTIME_ID, testTable, numberOfElementsInTest, blockSizeInTest);
  printf("\tSYST: %f sec\n", timeDifference);
  fprintf(f, "\tSYST: %f sec\n", timeDifference);

  fclose(f);

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

int parseOperationsStrategy(int numberOfElements, char * typeOfOperations, int * quantityOfOperations, char * arg) {
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
    if(numberOfElements < quantityOfOperations[i]) {
      if(typeOfOperations[i] == 'm' || typeOfOperations[i] == 'a' || typeOfOperations[i] == 'd') {
        printf("Too large amount of blocks to operate on in 4. argument\n");
        return -1;
      }
    }
  }
}

/**
 * Testing functions
 */
double getTimeCreateTable(clockid_t clk_id, int numberOfElementsInTest, int blockSizeInTest) {
  /* Variables */
  struct timespec timeStart, timeEnd;
  char ** testTable;
  int i;

  clock_gettime(clk_id, &timeStart);
  testTable = createDynamicTable(numberOfElementsInTest);
  for(i = 0; i < numberOfElementsInTest; i++)
    addBlockToDynamicTable(testTable, numberOfElementsInTest, createBlock(blockSizeInTest), i);
  clock_gettime(clk_id, &timeEnd);
  return (timeEnd.tv_sec - timeStart.tv_sec)
    + (timeEnd.tv_nsec - timeStart.tv_nsec) / BILLION;
}

double getTimeFindElement(clockid_t clk_id, char ** testTable, int numberOfElementsInTest) {
  /* Variables */
  struct timespec timeStart, timeEnd;

  clock_gettime(clk_id, &timeStart);
  findBlockWithSpecifiedQuantityInDynamicTable(testTable, numberOfElementsInTest, 0);
  clock_gettime(clk_id, &timeEnd);
  return (timeEnd.tv_sec - timeStart.tv_sec)
    + (timeEnd.tv_nsec - timeStart.tv_nsec) / BILLION;
}

double getTimeDeleteHalfAddHalf(clockid_t clk_id, char ** testTable, int numberOfElementsInTest, int blockSizeInTest) {
  /* Variables */
  struct timespec timeStart, timeEnd;
  int i;

  clock_gettime(clk_id, &timeStart);
  for(i = 0; i < numberOfElementsInTest / 2; i++)
    removeBlockFromDynamicTable(testTable, numberOfElementsInTest, i);
  for(i = 0; i < numberOfElementsInTest / 2; i++)
    addBlockToDynamicTable(testTable, numberOfElementsInTest, createBlock(blockSizeInTest), i);
  clock_gettime(clk_id, &timeEnd);
  return (timeEnd.tv_sec - timeStart.tv_sec)
    + (timeEnd.tv_nsec - timeStart.tv_nsec) / BILLION;
}

double getTimeReplaceHalf(clockid_t clk_id, char ** testTable, int numberOfElementsInTest, int blockSizeInTest) {
  /* Variables */
  struct timespec timeStart, timeEnd;
  int i;

  clock_gettime(clk_id, &timeStart);
  for(i = 0; i < numberOfElementsInTest / 2; i++) {
    removeBlockFromDynamicTable(testTable, numberOfElementsInTest, i);
    addBlockToDynamicTable(testTable, numberOfElementsInTest, createBlock(blockSizeInTest), i);
  }
  clock_gettime(clk_id, &timeEnd);
  return (timeEnd.tv_sec - timeStart.tv_sec)
    + (timeEnd.tv_nsec - timeStart.tv_nsec) / BILLION;
}
