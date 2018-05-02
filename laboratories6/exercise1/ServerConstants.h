#ifndef SERVER_CONSTANTS_H
#define SERVER_CONSTANTS_H

#define PROJ_NAME 'M'
#define MSGMAX 1000
#define GET_LATEST 0
#define MAX_CLIENTS 5

#define TRUE 1
#define FALSE 0

/**
  -- IMPORTANT --
    It is needed to create structure that begins with values that has
    constant size (such as int/char) and only then include arrays.
    I haven't found it in any documentation but when I had char table
    (mtext) between long and int (mtype, pid) then I've came up to
    bug that doesn't correctly shows pid value.  
 */
struct msgbuf {
  long mtype;
  int pid;
  char mtext[MSGMAX];
};

typedef enum {
  CLI_QUEUE = 'q',
  MIRROR = 'm',
  CALC = 'c',
  TIME = 't',
  END = 'e'
}
typeOfOperation;

#endif
