/**
  File: LibraryToWorkOnTable.c
 */

#include <limits.h>
#include <stdlib.h>
#include "LibraryToWorkOnTable.h"

/* For testing */
#include <stdio.h>

char ** createTable(int cellsQuantity) {
  return calloc(cellsQuantity, sizeof(char *));
}

void deleteTable(char ** table, int cellsQuantity) {
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

int addBlockToTable(char ** table, int cellsQuantity, char * block, int cell) {
  if(cell > cellsQuantity - 1 || cell < 0) return -1;
  int charCounter = getStringSize(block);
  table[cell] = calloc(charCounter, sizeof(char));
  int i;
  for(i = 0; i < charCounter; i++) {
    table[cell][i] = block[i];
  }
  return 0;
}

int removeBlockFromTable(char ** table, int cellsQuantity, int cell) {
  if(cell > cellsQuantity - 1 || cell < 0) return -1;
  free(table[cell]);
  table[cell] = calloc(1, sizeof(char *));
  return 0;
}

char * findBlockWithSpecifiedQuantity(char ** table, int cellsQuantity, int quantity) {
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

// int main() {
//   char ** a = createTable(5);
//   addBlockToTable(a, 5, "Jaga", 0);
//   // addBlockToTable(a, 5, "Jagalat", 3);
//   // removeBlockFromTable(a, 5, 0);
//   char * b = findBlockWithSpecifiedQuantity(a, 5, 7);
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
//   deleteTable(a, 5);
//   return 0;
// }
