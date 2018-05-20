#ifndef STRUCTURES_H
#define STRUCTURES_H

#define PROJ_NAME 'M'
#define SEMAPHORE_FLAGS 4
// http://www.cplusplus.com/forum/unices/23437/

typedef struct fifo fifo;
struct fifo {
  int id;
  int invited;
};

typedef enum {
  INVITE = 0,
  FINISH = 1,
  SLEEP = 2
  }
semaphoreFlags;

typedef enum {
  OP_SLEEP = 0,
  OP_WAKE_UP = 1,
  OP_INVITE = 2,
  OP_BEGIN_SHAVE = 3,
  OP_END_SHAVE = 4
  }
typeOfBarberOperation;

typedef enum {
  OP_WAKE_UP_BARBER = 0,
  OP_TAKE_SEAT_TO_SHAVE = 1,
  OP_LEAVE_AFTER_SHAVE = 2,
  OP_TAKE_SEAT_IN_QUEUE = 3,
  OP_LEAVE_WITHOUT_SHAVE = 4
  }
typeOfClientOperation;

#endif
