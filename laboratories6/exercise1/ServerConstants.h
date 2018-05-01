#ifndef SERVER_CONSTANTS_H
#define SERVER_CONSTANTS_H

#define PROJ_NAME 'M'
#define MSGMAX 4096
#define GET_LATEST 0

struct msgbuf {
  long mtype;
  char mtext[MSGMAX];
  int pid;
};

typedef enum {
  MIRROR = 'm',
  CALC = 'c',
  TIME = 't',
  END = 'e'
}
typeOfOperation;

#endif
