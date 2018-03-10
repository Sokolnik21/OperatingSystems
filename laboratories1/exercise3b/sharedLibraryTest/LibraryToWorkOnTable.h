#ifndef LIBRARY_TO_WORK_ON_TABLE_H
#define LIBRARY_TO_WORK_ON_TABLE_H
/**
  File: LibraryToWorkOnTable.h
 */

int getStringSize(char * block);

/**
 * Part of the exercise that works on static table
 */
#define MAX_STATIC_TABLE_SIZE 1000

/**
 * <TheMoreYouKnow>
 * Removed static from staticTable definition
 * - with static identifier this table cannot be modified in programs
 * that uses this library
 */
char staticTable[MAX_STATIC_TABLE_SIZE][MAX_STATIC_TABLE_SIZE];

/* Equivalent for creating and deleting dynamic table */
void initializeStaticTable();

int addBlockToStaticTable(char * block, int cell);
int removeBlockFromStaticTable(int cell);

char * findBlockWithSpecifiedQuantityInStaticTable(int quantity);

/**
 * Part of the exercise that works on dynamic table
 */
char ** createDynamicTable(int cellsQuantity);
void deleteDynamicTable(char ** table, int cellsQuantity);

int addBlockToDynamicTable(char ** table, int cellsQuantity, char * block, int cell);
int removeBlockFromDynamicTable(char ** table, int cellsQuantity, int cell);

char * findBlockWithSpecifiedQuantityInDynamicTable(char ** table, int cellsQuantity, int quantity);

#endif
