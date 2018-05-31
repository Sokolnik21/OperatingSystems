#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

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

void parseImageSize(int * cols, int * rows, char * line);
void parseImagePixels(int ** baseImage, int cols, int rows, int * currIndex, char * line);

void endWithError(int n);
void endWithTextError(char * txt, int n);

struct image parseImage(char * fileName);
struct filter parseFilter(char * fileName);

void parseFilterPixels(double ** matrix, int side, int * currIndex, char * line);

void DBG_printImage(struct image im);
void DBG_printFilter(struct filter ftr);

struct image prepareOutputImage(struct image base);
void updateOutputImage(struct image output, int row, int col, struct image base, struct filter ftr);

void saveOutputImage(struct image output, char * outputImageName);

int main(int argc, char * argv[]) {
  if(argc != 5)
    endWithTextError("Wrong amount of arguments", -1);

  /* Variables */
  struct image * base;
  struct filter * ftr;
  struct image * output;
  char * baseImageName;
  char * filterName;
  char * outputImageName;

  /* Parsing varialbes */
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

  DBG_printImage(* base);
  DBG_printFilter(* ftr);

  /* Prepare outputFile */
  output = malloc(sizeof(struct image));
  * output = prepareOutputImage(* base);

  int i, j;
  for(i = 0; i < output -> row ; i++)
    for(j = 0; j < output -> col ; j++)
      updateOutputImage(* output, i, j, * base, * ftr);

  // DBG_printImage(* output);

  saveOutputImage(* output, outputImageName);


  return 0;
}

void saveOutputImage(struct image output, char * outputImageName) {
  // printf("saving time\n");
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

void updateOutputImage(struct image output, int row, int col, struct image base, struct filter ftr) {
  /**
    . . . .
    . C . .   . . .
    . . . .   . C .
    . . . . , . . .
   */
  int ftrCenter = (ftr.side - 1) / 2;
  // printf("ftrCenter %d\n", ftrCenter);

  double result = 0;
  int i, j;
  for(i = 0; i < ftr.side; i++)
    for(j = 0; j < ftr.side; j++) {
      // printf("Parameters: %d, %d\n", min(base.row - 1, max(0, row - ftrCenter + i)), min(base.col - 1, max(0, col - ftrCenter + j)));
      // printf("Value: %d\n", base.matrix[min(base.row - 1, max(0, row - ftrCenter + i))][min(base.col - 1, max(0, col - ftrCenter + j))]);
      // printf("Filter: %f\n", ftr.matrix[i][j]);
      result += (base.matrix[min(base.row - 1, max(0, row - ftrCenter + i))][min(base.col - 1, max(0, col - ftrCenter + j))] * ftr.matrix[i][j]);
    }
  output.matrix[row][col] = (int)round(result);
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

struct image parseImage(char * fileName) {
  struct image result;
  /* Get information from files */
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
  /* Get information from files */
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

void parseFilterPixels(double ** matrix, int side, int * currIndex, char * line) {
  char * tmp = strtok(line, " ");
  while(tmp != NULL) {
    matrix[* currIndex / side][* currIndex % side] = atof(tmp);
    * currIndex = * currIndex + 1;
    tmp = strtok(NULL, " \n");
  }
}

void parseImageSize(int * cols, int * rows, char * line) {
  char * tmp = strtok(line, " ");
  * cols = atoi(tmp);
  tmp = strtok(NULL, " ");
  * rows = atoi(tmp);
}

void parseImagePixels(int ** baseImage, int cols, int rows, int * currIndex, char * line) {
  char * tmp = strtok(line, " ");
  while(tmp != NULL) {
    baseImage[* currIndex / cols][* currIndex % cols] = atoi(tmp);
    * currIndex = * currIndex + 1;
    tmp = strtok(NULL, " \n");
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
