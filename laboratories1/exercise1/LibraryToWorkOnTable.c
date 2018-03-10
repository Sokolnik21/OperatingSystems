/**
  File: LibraryToWorkOnTable.c
 */

#include <stdlib.h>
#include "LibraryToWorkOnTable.h"

int getStringSize(char * block) {
  int charCounter = 0;
  while(block[charCounter] != '\0')
    charCounter++;
  return charCounter;
}

/**
 * Part of the exercise that works on static table
 */
void initializeStaticTable() {
  int i;
  for(i = 0; i < MAX_STATIC_TABLE_SIZE; i++) {
    int j;
    for(j = 0; j < MAX_STATIC_TABLE_SIZE; j++)
      staticTable[i][j] = '\0';
  }
}

int addBlockToStaticTable(char * block, int cell) {
  if(cell > MAX_STATIC_TABLE_SIZE - 1 || cell < 0) return -1;

  /* Clearing table before addition */
  removeBlockFromStaticTable(cell);

  int charCounter = getStringSize(block);
  int i;
  for(i = 0; i < charCounter; i++)
    staticTable[cell][i] = block[i];

  return 0;
}

int removeBlockFromStaticTable(int cell) {
  if(cell > MAX_STATIC_TABLE_SIZE - 1 || cell < 0) return -1;
  int i;
  for(i = 0; i < MAX_STATIC_TABLE_SIZE; i++)
    staticTable[cell][i] = '\0';

  return 0;
}

char * findBlockWithSpecifiedQuantityInStaticTable(int quantity) {
  int foundCell = 0;
  int blockAsciiSumTmp = 0;
  int stringSize;

  int bestDifference;
  int tmpDifference;
  bestDifference = tmpDifference = 0;

  int i;
  int j;
  for(i = 0; i < MAX_STATIC_TABLE_SIZE; i++) {
    if(staticTable[i][0] == '\0')
      continue;
    stringSize = getStringSize(staticTable[i]);
    for(j = 0; j < stringSize; j++) {
      blockAsciiSumTmp += (int) staticTable[i][j];
    }
    tmpDifference = blockAsciiSumTmp > quantity ? blockAsciiSumTmp - quantity : quantity - blockAsciiSumTmp;
    if(tmpDifference < bestDifference) {
      foundCell = i;
      bestDifference = tmpDifference;
    }
    blockAsciiSumTmp = 0;
  }
  return staticTable[foundCell];
}

/**
 * Part of the exercise that works on dynamic table
 */
char ** createDynamicTable(int cellsQuantity) {
  return calloc(cellsQuantity, sizeof(char *));
}

void deleteDynamicTable(char ** table, int cellsQuantity) {
  int i;
  for(i = 0; i < cellsQuantity; i++)
    free(table[i]);
  free(table);
}

int addBlockToDynamicTable(char ** table, int cellsQuantity, char * block, int cell) {
  if(cell > cellsQuantity - 1 || cell < 0) return -1;
  int charCounter = getStringSize(block);
  table[cell] = calloc(charCounter, sizeof(char));
  int i;
  for(i = 0; i < charCounter; i++) {
    table[cell][i] = block[i];
  }
  return 0;
}

int removeBlockFromDynamicTable(char ** table, int cellsQuantity, int cell) {
  if(cell > cellsQuantity - 1 || cell < 0) return -1;
  free(table[cell]);
  table[cell] = calloc(1, sizeof(char *));
  return 0;
}

char * findBlockWithSpecifiedQuantityInDynamicTable(char ** table, int cellsQuantity, int quantity) {
  int foundCell = 0;
  int blockAsciiSumTmp = 0;
  int stringSize;

  int bestDifference;
  int tmpDifference;
  bestDifference = tmpDifference = 0;

  int i;
  int j;
  for(i = 0; i < cellsQuantity; i++) {
    if(table[i] == NULL)
      continue;
    stringSize = getStringSize(table[i]);
    for(j = 0; j < stringSize; j++) {
      blockAsciiSumTmp += (int) table[i][j];
    }
    tmpDifference = blockAsciiSumTmp > quantity ? blockAsciiSumTmp - quantity : quantity - blockAsciiSumTmp;
    if(tmpDifference < bestDifference) {
      foundCell = i;
      bestDifference = tmpDifference;
    }
    blockAsciiSumTmp = 0;
  }
  return table[foundCell];
}
