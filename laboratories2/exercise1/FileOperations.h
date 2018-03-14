#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H
/**
  File: FileOperations.h
 */

/**
To use this function it is needed to set dependation
for random number generator in main function
*/
char * createRecord(int recSize);

/* Functions that helps with sorting */
int compareRecords(char * recordA, char * recordB, int recSize);

/**
 * Part of the exercise that works with system functions
 */
void generateSys(char * fileName, int recQuan, int recSize);
void sortSys(char * fileName, int recQuan, int recSize);
void copySys(char * fileInputName, char * fileOutputName, int recSize);

/**
 * Part of the exercise that works with system functions
 */
void generateLib(char * fileName, int recQuan, int recSize);
void sortLib(char * fileName, int recQuan, int recSize);
void copyLib(char * fileInputName, char * fileOutputName, int recSize);

#endif
