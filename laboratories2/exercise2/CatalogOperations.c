#include <dirent.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// For chdir()
#include <unistd.h>
// for stat()
#include <sys/stat.h>

#include <time.h>

// void recursiveCatalogSearch() {
//   while((dirStruct = readdir(dirp)) != NULL) {
//     if(dirStruct -> d_type == DT_REG) {
//       stat(dirStruct -> d_name, &buffer);
//       describeFile(buffer, dirStruct -> d_name);
//     }
//     if(dirStruct -> d_type == DT_DIR) {
//       printf("Catalog: %s\n", dirStruct -> d_name);
//     }
//   }
// }

// void takeCareOfTheCatalog(char * currPath, DIR * dirp) {
//   struct dirent * dirStruct;
//   // dirStruct = readdir(dirp);
//
//   chdir(currPath);
//
//   // One instantion for each function
//   static char pathBuffer[1024];
//   static char nextPathBuffer[1024];
//
//   // printf("%s\n", nextPathBuffer);
//
//   while((dirStruct = readdir(dirp)) != NULL) {
//     strcpy(pathBuffer, currPath);
//     if(dirStruct -> d_type == DT_REG) {
//       printf("Path burref: %s\n", pathBuffer);
//       printf("%s%s\n", currPath, dirStruct -> d_name);
//     }
//     if(dirStruct -> d_type == DT_DIR) {
//       strcpy(pathBuffer, currPath);
//       strcpy(nextPathBuffer, pathBuffer);
//       strcat(nextPathBuffer, "/");
//       printf("NEXT Path burref: %s\n", nextPathBuffer);
//       takeCareOfTheCatalog(nextPathBuffer, dirp);
//     }
//       // Go into that catalog and run takeCareOfTheCatalog(DIR * dirp)
//   }
//
//   // printf("I'm the one and only\n");
// }

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
char* formatdate(char * str, time_t val) {
  strftime(str, 20, "%d.%m.%Y %H:%M:%S", localtime(&val));
  return str;
}

void describeFile(struct stat buffer, char * path) {
  printf("%7.ld ", buffer.st_size);
  printf("%s ", getMode(buffer.st_mode));
  char date[20];
  printf("%s ", formatdate(date, buffer.st_mtime));
  printf("%s\n", path);
}

int main(int argc, char * argv) {
  DIR * dirp;
  dirp = opendir(".");

  struct dirent * dirStruct;
  dirStruct = readdir(dirp);

  struct stat buffer;

  while((dirStruct = readdir(dirp)) != NULL) {
    if(dirStruct -> d_type == DT_REG) {
      stat(dirStruct -> d_name, &buffer);
      describeFile(buffer, dirStruct -> d_name);
    }
    if(dirStruct -> d_type == DT_DIR) {
      printf("Catalog: %s\n", dirStruct -> d_name);
    }
  }

  closedir(dirp);
}
