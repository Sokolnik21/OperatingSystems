#include <limits.h>
#include <stdlib.h>

// // fork()
// #include <sys/types.h>
// #include <unistd.h>
// #include <sys/wait.h>
//
// // limits
// #include <sys/resource.h>
//
// #include <stdio.h>

#define BYTES_QUANTITY 1E9
#define MASAKA 9000

int fib(int n);
char * ripYourMemmoryApart();

int main(int argc, char * argv[]) {
  if(argc != 2)
    return -1;

  char * bigTable;

  switch(argv[1][0]) {
    case '0'  : fib(MASAKA);
                break;
    default   : bigTable = ripYourMemmoryApart();
                break;
  }

  return 0;
}

int fib(int n) {
  if(n == 0 || n == 1)
    return 1;
  return fib(n - 1) + fib(n - 2);
}

char * ripYourMemmoryApart() {
  char * bigTable = malloc(BYTES_QUANTITY * sizeof(char));
  int i;
  for(i = 0; i < BYTES_QUANTITY; i ++)
    bigTable[i] = 0;
  return bigTable;
}
