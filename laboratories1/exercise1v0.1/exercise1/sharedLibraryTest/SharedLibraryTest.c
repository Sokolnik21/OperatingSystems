/*
  It's not allowed to include static library like this:
    #include "LibraryToWorkOnTable.h"
  Static (and shared) libraries are linked during compilation and
  every functions is already visible in program.
 */
#include <stdio.h>

int main() {
  char ** a = createTable(5);
  addBlockToTable(a, 5, "Jaga", 0);
  addBlockToTable(a, 5, "Jagalat", 3);
  // // removeBlockFromTable(a, 5, 0);
  char * b = findBlockWithSpecifiedQuantity(a, 5, 7);
  printf("%s\n", a[0]);
  printf("%s\n", b);
  // // a[0] = malloc(sizeof(char) * 4);
  // // a[0][0] = 'a';
  // // a[0][1] = 'c';
  // // a[0][2] = 'b';
  // // a[0][3] = 'l';
  // // a[0][4] = 'm';
  // // a[0][5] = 'q';
  // // printf("\n%d\n", getStringSize(a[0]));
  // // // a[0] = "asdfas\n"; !!! NIEDOPUSZCZALNE !!!
  // // printf("\n%c\n", a[0][5]);
  // // // free(a[0]);
  // // // "asdfa\n";
  // //
  // // printf("%s", a[1]);
  // // // free(a[0]);
  // deleteTable(a, 5);
  // return 0;
  return 0;
}
