#define _XOPEN_SOURCE 500

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// For chdir() and stat()
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

// for nftw
#include <ftw.h>
#include <stdint.h>

#include <limits.h>

typedef enum {
  LOWER   = '<',
  HIGHER  = '>',
  EQUAL   = '=',
  ALL     = 'a'
  }
typeOfComparision;

/* Global variables */
typeOfComparision comparision;
char timeVal[20];

int process(const char * file, const struct stat * sb, int flag, struct FTW * s);

/**
 * Describing file
 */
void describeFile(const struct stat * buffer, char * path);
char * formatDate(char * str, time_t val);
char * getMode(long int mode);
// Check whether file meets requirements
int compareTime(const struct stat * buffer, char * timeVal, typeOfComparision comparision);

/**
 * Parsing functions
 */
int parseTypeOfComparision(typeOfComparision * comparision, char * arg);

/**
 * Main function
 */
int main(int argc, char * argv[]) {
  /* Checking amount of arguments */
  if(argc != 4) {
    printf("Wrong amount of arguments\n");
    return -1;
  }

  /* Variables */
  char bufPath[1024];
  // typeOfComparision comparision;
  // char timeVal[20];

  /* Parsing variables */
  strcpy(bufPath, argv[1]);
  parseTypeOfComparision(&comparision, argv[2]);
  strcpy(timeVal, argv[3]);

  nftw(bufPath, process, 1, FTW_CHDIR | FTW_PHYS);

  return 0;
}

int process(const char * file, const struct stat * sb, int flag, struct FTW * s) {
	const char * name = file + s -> base;

  char pathPrefix[PATH_MAX];
  realpath(".", pathPrefix);
  strcat(pathPrefix, "/");

  if(flag == FTW_F) {
    if(compareTime(sb, timeVal, comparision) == 0) {
      strcat(pathPrefix, name);
      describeFile(sb, pathPrefix);
    }
  }

	return 0;
}

/**
 * Describing file
 */
void describeFile(const struct stat * buffer, char * path) {
  printf("%7.ld ", buffer -> st_size);
  printf("%s ", getMode(buffer -> st_mode));
  char date[20];
  printf("%s ", formatDate(date, buffer -> st_mtime));
  printf("%s\n", path);
}

/* Stack overflow */
char * getMode(long int mode) {
  char * str = malloc(10 * sizeof(char));
  int i;
  for(i = 0; i < 9; i++)
    str[i] = '-';
  // String must end with '\0'
  str[9] = '\0';

  if ( mode & S_IRUSR ) str[0] = 'r';    /* 3 bits for user  */
  if ( mode & S_IWUSR ) str[1] = 'w';
  if ( mode & S_IXUSR ) str[2] = 'x';

  if ( mode & S_IRGRP ) str[3] = 'r';    /* 3 bits for group */
  if ( mode & S_IWGRP ) str[4] = 'w';
  if ( mode & S_IXGRP ) str[5] = 'x';

  if ( mode & S_IROTH ) str[6] = 'r';    /* 3 bits for other */
  if ( mode & S_IWOTH ) str[7] = 'w';
  if ( mode & S_IXOTH ) str[8] = 'x';

  return str;
}

/* Stack overflow */
char * formatDate(char * str, time_t val) {
  strftime(str, 20, "%Y.%m.%d/%H:%M:%S", localtime(&val));
  return str;
}

// Check whether file meets requirements
int compareTime(const struct stat * buffer, char * timeVal, typeOfComparision comparision) {
  char fileTime[20];
  formatDate(fileTime, buffer -> st_mtime);

  int result;

  switch (comparision) {
    case LOWER:   (strcmp(fileTime, timeVal) < 0) ? (result = 0) : (result = -1);
                  break;
    case HIGHER:  (strcmp(fileTime, timeVal) > 0) ? (result = 0) : (result = -1);
                  break;
    case EQUAL:   (strcmp(fileTime, timeVal) == 0) ? (result = 0) : (result = -1);
                  break;
    case ALL:     result = 0;
                  break;
  }
  return result;
}

/**
 * Parsing functions
 */
int parseTypeOfComparision(typeOfComparision * comparision, char * arg) {
  switch(arg[0]) {
    case 'l'  : * comparision = LOWER;
                return 0;
    case 'h'  : * comparision = HIGHER;
                return 0;
    case 'e'  : * comparision = EQUAL;
                return 0;
    case 'a'  : * comparision = ALL;
                return 0;
    default   : printf("Wrong comparision type\n");
                printf("2. argument should be 'l' (aka '<'), 'h' (aka '>') 'e' (aka '=') or 'a' (aka all)\n");
                exit(-2);
  }
}
