/**
  File: LibraryToWorkOnTable.c
 */

#include <limits.h>
#include <stdlib.h>
#include "LibraryToWorkOnTable.h"

/* For testing */
#include <stdio.h>

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

  /* Clear table before add */
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
  int blockSize, foundCell;
  blockSize = foundCell = 0;
  int absoluteTmp, absolute;
  absoluteTmp = 0;
  absolute = INT_MAX;
  int i;
  for(i = 0; i < MAX_STATIC_TABLE_SIZE; i++) {
    if(staticTable[i] == '\0')
      continue;
    blockSize = getStringSize(staticTable[i]);
    absoluteTmp = blockSize > quantity ? blockSize - quantity : quantity - blockSize;
    if(absoluteTmp < absolute) {
      foundCell = i;
      absolute = absoluteTmp;
    }
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

int getStringSize(char * block) {
  int charCounter = 0;
  while(block[charCounter] != '\0')
    charCounter++;
  return charCounter;
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
  int blockSize, foundCell;
  blockSize = foundCell = 0;
  int absoluteTmp, absolute;
  absoluteTmp = 0;
  absolute = INT_MAX;
  int i;
  for(i = 0; i < cellsQuantity; i++) {
    if(table[i] == NULL)
      continue;
    blockSize = getStringSize(table[i]);
    absoluteTmp = blockSize > quantity ? blockSize - quantity : quantity - blockSize;
    if(absoluteTmp < absolute) {
      foundCell = i;
      absolute = absoluteTmp;
    }
  }
  return table[foundCell];
}
//
// int main() {
//   /* Static test */
//   initializeStaticTable();
//   addBlockToStaticTable("Jaganala", 0);
//   int stringSize = getStringSize(staticTable[0]);
//   printf("%d\n", stringSize);
//   char * w = findBlockWithSpecifiedQuantityInStaticTable(7);
//   printf("w: %s\n", w);
//
//   // removeBlockFromStaticTable(0);
//   printf("%s\n", staticTable[0]);
//
//   /* Dynamic test */
//   char ** a = createDynamicTable(5);
//   addBlockToDynamicTable(a, 5, "Jaga", 0);
//   addBlockToDynamicTable(a, 5, "Jagalat", 3);
//   // removeBlockFromTable(a, 5, 0);
//   char * b = findBlockWithSpecifiedQuantityInDynamicTable(a, 5, 7);
//   printf("%s\n", a[0]);
//   printf("%s\n", b);
//   // a[0] = malloc(sizeof(char) * 4);
//   // a[0][0] = 'a';
//   // a[0][1] = 'c';
//   // a[0][2] = 'b';
//   // a[0][3] = 'l';
//   // a[0][4] = 'm';
//   // a[0][5] = 'q';
//   // printf("\n%d\n", getStringSize(a[0]));
//   // // a[0] = "asdfas\n"; !!! NIEDOPUSZCZALNE !!!
//   // printf("\n%c\n", a[0][5]);
//   // // free(a[0]);
//   // // "asdfa\n";
//   //
//   // printf("%s", a[1]);
//   // // free(a[0]);
//   deleteDynamicTable(a, 5);
//   return 0;
// }
