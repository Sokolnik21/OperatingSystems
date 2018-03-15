#include <dirent.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// For chdir()
#include <unistd.h>
// for stat()
#include <sys/stat.h>

#include <time.h>

void describeFile(struct stat buffer, char * path);
char * formatdate(char * str, time_t val);
char * getMode(long int mode);

// void recursiveCatalogSearch(char * currPath, DIR * dirp) {
void recursiveCatalogSearch(char * currPath) {
  // if(currPath != ".") {
  //   chdir("..");
  // }
  if(!(currPath[0] == '.' && currPath[1] == '\0')) {
    // chdir("..");
    printf("Hello\n");
    printf("[%s]\n", currPath);
  }


  char bufPath[1024];
  strcpy(bufPath, currPath);

  DIR * dirp;
  dirp = opendir(bufPath);
  chdir(bufPath);

  // printf("[%s]\n", bufPath);

  struct dirent * dirStruct;
  struct stat buffer;

  while((dirStruct = readdir(dirp)) != NULL) {
    // dirp = opendir(bufPath);
    // chdir(bufPath);

    if(dirStruct -> d_type == DT_REG) {
        // printf("[%s]\n", bufPath);

      stat(dirStruct -> d_name, &buffer);
      char tmpFile[1024];
      strcpy(tmpFile, bufPath);
      strcat(tmpFile, "/");
      strcat(tmpFile, dirStruct -> d_name);
      // printf("%s\n", tmpFile);
      describeFile(buffer, tmpFile);
    }
    if(dirStruct -> d_type == DT_DIR) {
      if(dirStruct -> d_name[0] == '.') // Get rid of . and ..
        continue;
      // printf("Catalog: %s\n", dirStruct -> d_name);
      char tmpPath[1024];
      strcpy(tmpPath, bufPath);
      strcat(tmpPath, "/");
      strcat(tmpPath, dirStruct -> d_name);
      // printf("%s\n", tmpPath);
      recursiveCatalogSearch(tmpPath);
    }
  }

  // dirp = opendir("..");
  /******/
  chdir("..");

  closedir(dirp);
}

int main(int argc, char * argv[]) {
  char bufPath[1024];
  strcpy(bufPath, "."); //Here will be 1. argument instead of "."

  recursiveCatalogSearch(bufPath);

  // DIR * dirp;
  // dirp = opendir(bufPath);
  // chdir(bufPath);
  //
  // struct dirent * dirStruct;
  // struct stat buffer;
  //
  // while((dirStruct = readdir(dirp)) != NULL) {
  //   if(dirStruct -> d_type == DT_REG) {
  //     stat(dirStruct -> d_name, &buffer);
  //     char tmpFile[1024];
  //     int i;
  //     for(i = 0; i < 1024; i++)
  //       tmpFile[i] = 0;
  //     strcpy(tmpFile, bufPath);
  //     strcat(tmpFile, "/");
  //     strcat(tmpFile, dirStruct -> d_name);
  //     describeFile(buffer, tmpFile);
  //   }
  //   if(dirStruct -> d_type == DT_DIR) {
  //     if(dirStruct -> d_name[0] == '.') // Get rid of . and ..
  //       continue;
  //     printf("Catalog: %s\n", dirStruct -> d_name);
  //     char tmpPath[1024];
  //     int i;
  //     for(i = 0; i < 1024; i++)
  //       tmpPath[i] = 0;
  //     strcpy(tmpPath, bufPath);
  //     strcat(tmpPath, "/");
  //     strcat(tmpPath, dirStruct -> d_name);
  //     printf("%s\n", tmpPath);
  //     recursiveCatalogSearch(tmpPath);
  //   }
  // }

  // closedir(dirp);
}


void describeFile(struct stat buffer, char * path) {
  printf("%7.ld ", buffer.st_size);
  printf("%s ", getMode(buffer.st_mode));
  char date[20];
  printf("%s ", formatdate(date, buffer.st_mtime));
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
char * formatdate(char * str, time_t val) {
  strftime(str, 20, "%d.%m.%Y %H:%M:%S", localtime(&val));
  return str;
}