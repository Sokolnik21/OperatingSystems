#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include <unistd.h>

enum differentiateType {
  equal = 'e',
  higher = 'h',
  lower = 'l'
};

enum displayType {
  silent = 's',
  verbose = 'v'
};

struct specification {
  int producerQuan;
  int clientQuan;
  int bufferSize;
  char * fileToRead;
  int differentiateNumber;
  enum differentiateType diffType;
  enum displayType disType;
  int endCondition;
};

struct buffer {
  char ** products;
  int currProd;
  int prodQuan;
  int MAX_PROD;
};

struct threadInfo {
  pthread_t threadId;
  struct specification * spec;
  struct buffer * prod;
};

struct specification parseSpecification(char * fileName);
struct buffer createBuffer(struct specification spec);

void bufferAdd(struct buffer * prod, char * product);
char * bufferRemove(struct buffer * prod);
int bufferIsFull(struct buffer prod);
int bufferIsEmpty(struct buffer prod);

void producerMethod(struct buffer * prod, struct specification spec);
void clientMethod(struct buffer * prod, struct specification spec);
static void * threadProducer(void * arg);
static void * threadClient(void * arg);

void DBG_printSpecification(struct specification spec);
void DBG_printBuffer(struct buffer prod);

void endWithError(int n);
void endWithTextError(char * txt, int n);

sem_t q_lock;
sem_t q_full;
sem_t q_empty;

int main(int argc, char * argv[]) {
  if(argc != 2)
    endWithTextError("Wrong amount of arguments", -1);

  struct specification spec = parseSpecification(argv[1]);
  struct buffer prod = createBuffer(spec);
  struct threadInfo * threadProd;
  struct threadInfo * threadClie;
  // DBG_printSpecification(spec);
  // DBG_printBuffer(prod);

  /* Initialize semaphores */
  sem_init(&q_lock, 0, 1);
  sem_init(&q_full, 0, 0);
  sem_init(&q_empty, 0, spec.bufferSize);

  /* Create threads */
  threadProd = malloc(spec.producerQuan * sizeof(struct threadInfo));
  threadClie = malloc(spec.clientQuan * sizeof(struct threadInfo));
  int i;
  for(i = 0; i < spec.producerQuan; i++) {
    threadProd[i].spec = &spec;
    threadProd[i].prod = &prod;
  }
  for(i = 0; i < spec.producerQuan; i++) {
    if(pthread_create(&threadProd[i].threadId, NULL, &threadProducer, &threadProd[i]) != 0)
      endWithError(-1);
  }
  for(i = 0; i < spec.clientQuan; i++) {
    threadClie[i].spec = &spec;
    threadClie[i].prod = &prod;
  }
  for(i = 0; i < spec.clientQuan; i++) {
    if(pthread_create(&threadClie[i].threadId, NULL, &threadClient, &threadClie[i]) != 0)
      endWithError(-1);
  }

  void * res;
  switch(spec.endCondition) {
    case 0 :
      /* Wait for threads to end */
      for(i = 0; i < spec.producerQuan; i++) {
        if(pthread_join(threadProd[i].threadId, &res) != 0)
        endWithError(-1);
      }
      break;
    default :
      sleep(spec.endCondition);
      break;
  }

  return 0;
}

struct specification parseSpecification(char * fileName) {
  struct specification spec;
  FILE * f = fopen(fileName, "r");
  /* Get information from file */
  char * line = NULL;
  char * tmp;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, f)) != -1) {
    if(line[0] == '#' || line[0] == '\n')
      continue;
    tmp = strtok(line, " ");
    if(strcmp(tmp, "producerQuan:") == 0) {
      tmp = strtok(NULL, " ");
      spec.producerQuan = atoi(tmp);
      continue;
    }
    if(strcmp(tmp, "clientQuan:") == 0) {
      tmp = strtok(NULL, " ");
      spec.clientQuan = atoi(tmp);
      continue;
    }
    if(strcmp(tmp, "bufferSize:") == 0) {
      tmp = strtok(NULL, " ");
      spec.bufferSize = atoi(tmp);
      continue;
    }
    if(strcmp(tmp, "fileToRead:") == 0) {
      tmp = strtok(NULL, " ");
      spec.fileToRead = malloc(strlen(tmp) * sizeof(char));
      tmp[strlen(tmp) - 1] = '\0';  // to get rid of '\n'
      strcpy(spec.fileToRead, tmp);
      continue;
    }
    if(strcmp(tmp, "differentiateNumber:") == 0) {
      tmp = strtok(NULL, " ");
      spec.differentiateNumber = atoi(tmp);
      continue;
    }
    if(strcmp(tmp, "differentiateType:") == 0) {
      tmp = strtok(NULL, " ");
      if(!(tmp[0] == 'e' || tmp[0] == 'h' || tmp[0] == 'l'))
        endWithTextError("Wrong differentiateType in Configuration.txt", -2);
      spec.diffType = tmp[0];
      continue;
    }
    if(strcmp(tmp, "displayType:") == 0) {
      tmp = strtok(NULL, " ");
      if(!(tmp[0] == 's' || tmp[0] == 'v'))
        endWithTextError("Wrong displayType argument in Configuration.txt", -2);
      spec.disType = tmp[0];
      continue;
    }
    if(strcmp(tmp, "endCondition:") == 0) {
      tmp = strtok(NULL, " ");
      spec.endCondition = atoi(tmp);
      continue;
    }
  }
  fclose(f);

  return spec;
}

struct buffer createBuffer(struct specification spec) {
  struct buffer prod;

  prod.MAX_PROD = spec.bufferSize;
  prod.currProd = 0;
  prod.prodQuan = 0;
  prod.products = malloc(spec.bufferSize * sizeof(char **));
  int i;
  for(i = 0; i < spec.bufferSize; i++) {
    prod.products[i] = malloc(sizeof(char *));
    prod.products[i][0] = '\0';
  }

  return prod;
}

void bufferAdd(struct buffer * prod, char * product) {
  int firstFreeSpace;
  firstFreeSpace = (prod -> currProd + prod -> prodQuan) % prod -> MAX_PROD;
  prod -> products[firstFreeSpace] = malloc((strlen(product) + 1) * sizeof(char));
  strcpy(prod -> products[firstFreeSpace], product);
  prod -> prodQuan = prod -> prodQuan + 1;
}

char * bufferRemove(struct buffer * prod) {
  char * item;
  item = malloc((strlen(prod -> products[prod -> currProd]) + 1) * sizeof(char));
  strcpy(item, prod -> products[prod -> currProd]);
  free(prod -> products[prod -> currProd]);
  prod -> products[prod -> currProd] = malloc(sizeof(char *));
  prod -> products[prod -> currProd][0] = '\0';
  prod -> currProd = (prod -> currProd + 1) % prod -> MAX_PROD;
  prod -> prodQuan = prod -> prodQuan - 1;
  return item;
}

int bufferIsFull(struct buffer prod) {
  return prod.prodQuan == prod.MAX_PROD;
}

int bufferIsEmpty(struct buffer prod) {
  return prod.prodQuan == 0;
}

void producerMethod(struct buffer * prod, struct specification spec) {
  FILE * f = fopen(spec.fileToRead, "r");
  /* Get information from file */
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, f)) != -1) {
    sem_wait(&q_empty);
    sem_wait(&q_lock);
    bufferAdd(prod, line);
    int buffIndex = (prod -> currProd + prod -> prodQuan) % prod -> MAX_PROD;
    switch(spec.disType) {
      case silent :
        break;
      case verbose :
        printf("Added product at index: %d\n", buffIndex);
        break;
    }
    sem_post(&q_lock);
    sem_post(&q_full);
  }
  free(line);
  fclose(f);
}

void clientMethod(struct buffer * prod, struct specification spec) {
  char * item;
  int diff, result;
  while (1) {
    sem_wait(&q_full);
    sem_wait(&q_lock);
    item = bufferRemove(prod);
    diff = spec.differentiateNumber - strlen(item);
    switch(spec.diffType) {
      case equal :
        result = diff == 0;
        break;
      case higher :
        result = diff < 0;
        break;
      case lower :
        result = diff > 0;
        break;
    }
    int buffIndex = (prod -> currProd + prod -> MAX_PROD - 1) % prod -> MAX_PROD;
    if(result) {
      // printf("Found product that meets requirement\n");
      printf("Buff index %d; text: %s", buffIndex, item);
    }
    switch(spec.disType) {
      case silent :
        break;
      case verbose :
        result ? printf("Found product meets requirements: ") :
          printf("Found product does not meet requirements: ");
        printf("diffType %d, foundProd %ld\n", spec.differentiateNumber, strlen(item));
        break;
    }
    free(item);
    sem_post(&q_lock);
    sem_post(&q_empty);
  }
}

static void * threadProducer(void * arg) {
  struct threadInfo * tinfo = arg;
  producerMethod(tinfo -> prod, * tinfo -> spec);
  return 0;
}

static void * threadClient(void * arg) {
  struct threadInfo * tinfo = arg;
  clientMethod(tinfo -> prod, * tinfo -> spec);
  return 0;
}

void DBG_printSpecification(struct specification spec) {
  printf("producerQuan %d\n", spec.producerQuan);
  printf("clientQuan %d\n", spec.clientQuan);
  printf("bufferSize %d\n", spec.bufferSize);
  printf("differentiateNumber %d\n", spec.differentiateNumber);
  printf("fileToRead %s\n", spec.fileToRead);
  printf("diffType %c\n", spec.diffType);
  printf("disType %c\n", spec.disType);
}

void DBG_printBuffer(struct buffer prod) {
  printf("MAX_PROD %d\n", prod.MAX_PROD);
  printf("currProd %d\n", prod.currProd);
  printf("prodQuan %d\n", prod.prodQuan);
  int i;
  for(i = 0; i < prod.MAX_PROD; i++)
    if(prod.products[i][0] != '\0')
      printf("%s\n", prod.products[i]);
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

void endWithTextError(char * txt, int n) {
  printf("Error: %s\n", txt);
  exit(n);
}
