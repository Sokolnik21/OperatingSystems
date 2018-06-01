#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

struct image {
  int row;
  int col;
  int depth;
  int ** matrix;
};

struct filter {
  int side;
  double ** matrix;
};

struct threadInfo {
  pthread_t threadId;
  /* arg[0] = thread id in application; arg[1] = total threads in application */
  int arg[2];
};

/* Global variables */
struct image * base;
struct filter * ftr;
struct image * output;

int max(int a, int b);
int min(int a, int b);

struct image parseImage(char * fileName);
struct filter parseFilter(char * fileName);
void parseImageSize(int * cols, int * rows, char * line);
void parseImagePixels(int ** baseImage, int cols, int rows, int * currIndex, char * line);
void parseFilterPixels(double ** matrix, int side, int * currIndex, char * line);

struct image prepareOutputImage(struct image base);
void updateOutputImage(struct image output, int row, int col, struct image base, struct filter ftr);

static void * threadFunction(void * arg);

void printResults(struct image base, struct filter ftr, int threadQuan, double elapsed);
void saveOutputImage(struct image output, char * outputImageName);

void DBG_printImage(struct image im);
void DBG_printFilter(struct filter ftr);

void endWithError(int n);
void endWithTextError(char * txt, int n);

int main(int argc, char * argv[]) {
  if(argc != 5)
    endWithTextError("Wrong amount of arguments", -1);

  /* Variables */
  char * baseImageName;
  char * filterName;
  char * outputImageName;
  int threadQuan;
  struct timespec start, finish;
  double elapsed;

  /* Parsing varialbes */
  threadQuan = atoi(argv[1]);
  baseImageName = malloc(strlen(argv[2]) * sizeof(char));
  strcpy(baseImageName, argv[2]);
  filterName = malloc(strlen(argv[3]) * sizeof(char));
  strcpy(filterName, argv[3]);
  outputImageName = malloc(strlen(argv[4]) * sizeof(char));
  strcpy(outputImageName, argv[4]);

  /* Filling images and filters */
  base = malloc(sizeof(struct image));
  * base = parseImage(baseImageName);
  ftr = malloc(sizeof(struct filter));
  * ftr = parseFilter(filterName);

  // DBG_printImage(* base);
  // DBG_printFilter(* ftr);

  /* Prepare outputFile */
  output = malloc(sizeof(struct image));
  * output = prepareOutputImage(* base);

  clock_gettime(CLOCK_MONOTONIC, &start);

  /* With threads */
  /* Create threads */
  struct threadInfo * threads;
  threads = malloc(threadQuan * sizeof(struct threadInfo));
  int i;
  for(i = 0; i < threadQuan; i++) {
    threads[i].arg[0] = i;
    threads[i].arg[1] = threadQuan;
  }
  for(i = 0; i < threadQuan; i++) {
    if(pthread_create(&threads[i].threadId, NULL, &threadFunction, &threads[i]) != 0)
      endWithError(-1);
  }
  /* Wait for threads to end */
  void * res;
  for(i = 0; i < threadQuan; i++) {
    if(pthread_join(threads[i].threadId, &res) != 0)
      endWithError(-1);
  }

  clock_gettime(CLOCK_MONOTONIC, &finish);
  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1e9;

  printResults(* base, * ftr, threadQuan, elapsed);

  // /* Without threads */
  // int i, j;
  // for(i = 0; i < output -> row ; i++)
  //   for(j = 0; j < output -> col ; j++)
  //     updateOutputImage(* output, i, j, * base, * ftr);
  // DBG_printImage(* output);

  saveOutputImage(* output, outputImageName);

  return 0;
}

int max(int a, int b) {
  int result;
  if(a > b)
    result = a;
  else
    result = b;
  return result;
}

int min(int a, int b) {
  int result;
  if(a < b)
    result = a;
  else
    result = b;
  return result;
}

struct image parseImage(char * fileName) {
  struct image result;
  /* Get information from file */
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int lineCounter = 0;
  int currIndex = 0;
  FILE * f = fopen(fileName, "r");
  while ((read = getline(&line, &len, f)) != -1) {
    if(line[0] == '#' || line[0] == '\n')
      continue;
    if(lineCounter < 4)
      lineCounter++;
    if(lineCounter == 1)
      continue;
    if(lineCounter == 2) {
      parseImageSize(&result.col, &result.row, line);
      result.matrix = malloc(result.row * sizeof(int *));
      int iter;
      for(iter = 0; iter < result.row; iter++)
        result.matrix[iter] = malloc(result.col * sizeof(int));
    }
    if(lineCounter == 3)
      result.depth = atoi(line);
    if(lineCounter == 4) {
      parseImagePixels(result.matrix, result.col, result.row, &currIndex, line);
    }
  }
  fclose(f);

  return result;
}

struct filter parseFilter(char * fileName) {
  struct filter result;
  /* Get information from file */
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int lineCounter = 0;
  int currIndex = 0;
  FILE * f = fopen(fileName, "r");
  while ((read = getline(&line, &len, f)) != -1) {
    if(line[0] == '#' || line[0] == '\n')
      continue;
    if(lineCounter < 2)
      lineCounter++;
    if(lineCounter == 1) {
      result.side = atoi(line);
      result.matrix = malloc(result.side * sizeof(double *));
      int iter;
      for(iter = 0; iter < result.side; iter++)
        result.matrix[iter] = malloc(result.side * sizeof(double));
    }
    if(lineCounter == 2) {
      parseFilterPixels(result.matrix, result.side, &currIndex, line);
    }
  }
  fclose(f);

  return result;
}

void parseImageSize(int * cols, int * rows, char * line) {
  char * tmp = strtok(line, " ");
  * cols = atoi(tmp);
  tmp = strtok(NULL, " ");
  * rows = atoi(tmp);
}

void parseImagePixels(int ** baseImage, int cols, int rows, int * currIndex, char * line) {
  char * tmp = strtok(line, " \n");
  while(tmp != NULL && (* currIndex != cols * rows)) {
    baseImage[* currIndex / cols][* currIndex % cols] = atoi(tmp);
    * currIndex = * currIndex + 1;
    tmp = strtok(NULL, " \n");
  }
}

void parseFilterPixels(double ** matrix, int side, int * currIndex, char * line) {
  char * tmp = strtok(line, " ");
  while(tmp != NULL) {
    matrix[* currIndex / side][* currIndex % side] = atof(tmp);
    * currIndex = * currIndex + 1;
    tmp = strtok(NULL, " \n");
  }
}

struct image prepareOutputImage(struct image base) {
  struct image output;

  output.row = base.row;
  output.col = base.col;
  output.depth = base.depth;
  output.matrix = malloc(output.row * sizeof(int *));
  int iter;
  for(iter = 0; iter < output.row; iter++)
    output.matrix[iter] = malloc(output.col * sizeof(int));

  return output;
}

void updateOutputImage(struct image output, int row, int col, struct image base, struct filter ftr) {
  /** ftrCenter == C
    . . . .
    . C . .   . . .
    . . . .   . C .
    . . . . , . . .
   */
  int ftrCenter = (ftr.side - 1) / 2;
  double result = 0;
  int i, j;
  for(i = 0; i < ftr.side; i++)
    for(j = 0; j < ftr.side; j++)
      result += (base.matrix[min(base.row - 1, max(0, row - ftrCenter + i))][min(base.col - 1, max(0, col - ftrCenter + j))] * ftr.matrix[i][j]);
  output.matrix[row][col] = (int)round(result);
}

static void * threadFunction(void * arg) {
  struct threadInfo * tinfo = arg;
  int threadNum = tinfo -> arg[0];
  int totalThreads = tinfo -> arg[1];

  int i, currRow, currCol;
  for(i = threadNum; i < (base -> row * base -> col); i += totalThreads) {
    currRow = i / (base -> col);
    currCol = i % (base -> col);
    updateOutputImage(* output, currRow, currCol, * base, * ftr);
  }
  // printf("%d. thread ended\n", threadNum + 1);
}

void printResults(struct image base, struct filter ftr, int threadQuan, double elapsed) {
  printf("Measurement\n");
  printf("\tThreads: %d\n", threadQuan);
  printf("\tBase image size: %dx%d\n", base.col, base.row);
  printf("\tFilter size: %dx%d\n", ftr.side, ftr.side);
  printf("Time\n");
  printf("\t%f\n", elapsed);
}

void saveOutputImage(struct image output, char * outputImageName) {
  FILE * f = fopen(outputImageName, "w");
  fprintf(f, "P2\n");
  fprintf(f, "%d %d\n", output.col, output.row);
  fprintf(f, "%d\n", output.depth);
  int i, j;
  for(i = 0; i < output.row ; i++) {
    for(j = 0; j < output.col ; j++)
      fprintf(f, "%d ", output.matrix[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
}

void DBG_printImage(struct image im) {
  printf("%d\n", im.row);
  printf("%d\n", im.col);
  printf("%d\n", im.depth);

  int q = 0;
  int w = 0;
  for(; q < im.row; q++) {
    for(w = 0; w < im.col; w++)
      printf("%2d ", im.matrix[q][w]);
    printf("\n");
  }
}

void DBG_printFilter(struct filter ftr) {
  printf("%d\n", ftr.side);

  int q = 0;
  int w = 0;
  for(; q < ftr.side; q++) {
    for(w = 0; w < ftr.side; w++)
      printf("%f ", ftr.matrix[q][w]);
    printf("\n");
  }
}

void endWithError(int n) {
  printf("Error: %s\n", strerror(errno));
  exit(n);
}

void endWithTextError(char * txt, int n) {
  printf("Error: %s\n", txt);
  exit(n);
}
