/**
  File: FileOperations.c
 */

/* Libraries to work on files with system functions */
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

/* Standard library to work on files */
#include <stdio.h>

/* Base libraries */
#include <time.h>
#include <stdlib.h>
#include "libraries/FileOperations.h"

/* Comparing Strings */
#include <string.h>

typedef enum {
  GENE = 'g',
  SORT = 's',
  COPY = 'c'
  }
typeOfOperation;

typedef enum {
  SYS = 's',
  LIB = 'l'
  }
operationStrategy;

/**
 * Operation standarise
 */
 void generate(operationStrategy strategy, char * fileName, int recQuan, int recSize);
 void sort(operationStrategy strategy, char * fileName, int recQuan, int recSize);
 void copy(operationStrategy strategy, char * fileInputName, char * fileOutputName, int recSize);

/**
 * Parsing functions
 */
int parseTypeOfOperation(typeOfOperation * operation, char * arg);
int parseOperationStrategy(operationStrategy * strategy, char * arg);

/**
 * Main function
 */
int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 6) {
    printf("Wrong amount of arguments\n");
    return -1;
  }

  /* Variables */
  int recQuan;
  int recSize;
  typeOfOperation operation;
  operationStrategy strategy;
  char * file;
  char * fileInput;
  char * fileOutput;

  /* Parsing variables */
  /* Parse type of operation */
  if(parseTypeOfOperation(&operation, argv[1]) == -1)
    return -2;
  /* Parse the rest variables based on type of operation */
  switch(operation) {
    case GENE :
    case SORT : file = argv[2];
                recQuan = atoi(argv[3]);
                recSize = atoi(argv[4]);
                parseOperationStrategy(&strategy, argv[5]);
                break;
    case COPY : fileInput = argv[2];
                fileOutput = argv[3];
                recSize = atoi(argv[4]);
                parseOperationStrategy(&strategy, argv[5]);
                break;
  }

  /* Setting dependation for random number generator */
  srand(time(NULL));

  switch(operation) {
    case GENE : generate(strategy, file, recQuan, recSize);
                break;
    case SORT : sort(strategy, file, recQuan, recSize);
                break;
    case COPY : copy(strategy, fileInput, fileOutput, recSize);
                break;
  }

  return 0;
}

/**
 * Operation standarise
 */
void generate(operationStrategy strategy, char * fileName, int recQuan, int recSize) {
  switch(strategy) {
    case SYS :  generateSys(fileName, recQuan, recSize);
                break;
    case LIB :  generateLib(fileName, recQuan, recSize);
                break;
  }
}

void sort(operationStrategy strategy, char * fileName, int recQuan, int recSize) {
 switch(strategy) {
   case SYS :  sortSys(fileName, recQuan, recSize);
               break;
   case LIB :  sortLib(fileName, recQuan, recSize);
               break;
  }
}

void copy(operationStrategy strategy, char * fileInputName, char * fileOutputName, int recSize) {
 switch(strategy) {
   case SYS :  copySys(fileInputName, fileOutputName, recSize);
               break;
   case LIB :  copyLib(fileInputName, fileOutputName, recSize);
               break;
  }
}

/**
 * Parsing functions
 */
int parseTypeOfOperation(typeOfOperation * operation, char * arg) {
  switch(arg[0]) {
    case 'g'  : * operation = GENE;
                return 0;
    case 's'  : * operation = SORT;
                return 0;
    case 'c'  : * operation = COPY;
                return 0;
    default   : printf("Wrong operation type\n");
                printf("1. argument should be \"generate\", \"sort\" or \"copy\"\n");
                exit(-2);
  }
}

int parseOperationStrategy(operationStrategy * strategy, char * arg) {
  switch(arg[0]) {
    case 's'  : * strategy = SYS;
                return 0;
    case 'l'  : * strategy = LIB;
                return 0;
    default   : printf("Wrong strategy type\n");
                printf("5. argument should be \"sys\" - system or \"lib\" - standard library\n");
                exit(-3);
  }
}
