#ifndef SERVER_CONSTANTS_H
#define SERVER_CONSTANTS_H

#define MSGMAX 1000
#define MAX_CLIENTS 5

#define TRUE 1
#define FALSE 0

struct msgContex {
  char type;
  char id;
  char text[MSGMAX - 2];
};

union bufStuff {
  char buffer[MSGMAX];
  struct msgContex contex;
};

typedef enum {
  CLI_QUEUE = 'q',
  MIRROR = 'm',
  CALC = 'c',
  TIME = 't',
  END = 'e'
}
typeOfOperation;

char * SERVER_PREFIX = "/queueS";
char * CLIENT_PREFIX = "/queueC";

#endif
