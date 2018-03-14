/**
  File: FileOperations.c
 */

/* Libraries to work on files with system functions */
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

/* Base libraries */
#include <time.h>
#include <stdlib.h>
#include "FileOperations.h"

/* Comparing Strings */
#include <string.h>
/* Test */
#include <stdio.h>

/**
 To use this function it is needed to set dependation
 for random number generator in main function
  ~ srand(time(NULL));
 */
char * createRecord(int recSize) {
  /* Allocating space for string [+ '\n'] */
  char * result = calloc(recSize + 1, sizeof(char));

  /* Creating an instance of random number generator */
  int rng;

  /* Set last cell with '\n' */
  result[recSize] = '\n';
  /* Set cells with [a..z] chars */
  while (recSize-- > 0) {
    rng = rand();
    result[recSize] = (char) (rng % ('z' - 'a' + 1)) + 'a';
  }

  return result;
}

/**
  recordA < recordB -> return 0
  recordA > recordB -> return 1
 */
int compareRecords(char * recordA, char * recordB, int recSize) {
  unsigned char charA;
  unsigned char charB;

  int i;
  for(i = 0; i < recSize; i++) {
    charA = (unsigned char) recordA[i];
    charB = (unsigned char) recordB[i];
    if(charA == charB) continue;
    if(charA < charB) {
      return 0;
    }
    else return 1;
  }
  return 0;
}

/**
* Part of the exercise that works with system functions
*/
void generateSys(char * fileName, int recQuan, int recSize) {
  int file;
  file = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  int i;
  for(i = 0; i < recQuan; i++) {
    write(file, createRecord(recSize), recSize + 1);
  }

  close(file);
}

void sortSys(char * fileName, int recQuan, int recSize) {
  /* Add place for '\n' char */
  int rowSize = recSize + 1;

  int file;
  file = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  char * bufferBest = calloc(recSize, sizeof(char));
  char * bufferTmp = calloc(recSize, sizeof(char));
  int bestCell;

  int i;
  for(i = 0; i < recQuan - 1; i++) {  // There is no need to iterate last cell
    bestCell = i;
    lseek(file, i * rowSize, SEEK_SET);
    read(file, bufferBest, recSize * sizeof(char));
    int j;
    for(j = i + 1; j < recQuan; j++) {
      lseek(file, j * rowSize, SEEK_SET);
      read(file, bufferTmp, recSize * sizeof(char));
      /* If(bufferBest > bufferTmp) */
      if(compareRecords(bufferBest, bufferTmp, recSize) == 1) {
        bestCell = j;
        strcpy(bufferBest, bufferTmp);
      }
    }
    /* Swap records */
    if(bestCell != i) {
      lseek(file, i * rowSize, SEEK_SET);
      read(file, bufferTmp, recSize * sizeof(char));

      lseek(file, i * rowSize, SEEK_SET);
      write(file, bufferBest, recSize * sizeof(char));

      lseek(file, bestCell * rowSize, SEEK_SET);
      write(file, bufferTmp, recSize * sizeof(char));
    }
  }

  free(bufferBest);
  free(bufferTmp);
  close(file);
}

void copySys(char * fileInputName, char * fileOutputName, int recSize) {
  int fileInput, fileOutput;
  fileInput = open(fileInputName, O_RDONLY);
  fileOutput = open(fileOutputName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

  int bufferSize;
  char * buffer = calloc(recSize, sizeof(char));
  while((bufferSize = read(fileInput, buffer, sizeof(buffer))) > 0)
    write(fileOutput, buffer, bufferSize);

  free(buffer);
  close(fileInput);
  close(fileOutput);
}

/**
* Part of the exercise that works with system functions
*/
void generateLib();

/**
 * Main function
 */
int main(int argc, char * argv[]) {
  /* Setting dependation for random number generator */
  srand(time(NULL));

  generateSys("test.txt", 5, 5);

  copySys("test.txt", "copy.txt", 5);
  sortSys("copy.txt", 5, 5);
  // printf("%s\n", createRecord(5));
  return 0;
}
