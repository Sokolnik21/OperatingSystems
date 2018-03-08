#ifndef LIBRARY_TO_WORK_ON_TABLE_H
#define LIBRARY_TO_WORK_ON_TABLE_H
/**
  File: LibraryToWorkOnTable.h
 */

char ** createTable(int cellsQuantity);
void deleteTable(char ** table, int cellsQuantity);

int getStringSize(char * block);

int addBlockToTable(char ** table, int cellsQuantity, char * block, int cell);
int removeBlockFromTable(char ** table, int cellsQuantity, int cell);

char * findBlockWithSpecifiedQuantity(char ** table, int cellsQuantity, int quantity);

#endif
