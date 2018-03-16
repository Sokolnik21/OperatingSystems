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
char * formatDate(char * str, time_t val);
char * getMode(long int mode);

void recursiveCatalogSearch(char * currDirName, char * showFilePath) {
  char * currPath = ".";
  char bufPath[1024];
  strcpy(bufPath, currPath);

  char sfp[1024];
  strcpy(sfp, showFilePath);

  DIR * dirp;
  dirp = opendir(currDirName);

  chdir(currDirName);

  struct dirent * dirStruct;
  struct stat buffer;

  while((dirStruct = readdir(dirp)) != NULL) {
    if(dirStruct -> d_type == DT_REG) {
      stat(dirStruct -> d_name, &buffer);
      char tmpFile[1024];
      strcpy(tmpFile, sfp);
      strcat(tmpFile, dirStruct -> d_name);
      describeFile(buffer, tmpFile);
    }
    if(dirStruct -> d_type == DT_DIR) {
      if(dirStruct -> d_name[0] == '.') // Get rid of . and ..
        continue;
      char tmpPath[1024];
      strcpy(tmpPath, bufPath);
      strcat(tmpPath, "/");
      strcat(tmpPath, dirStruct -> d_name);

      char tmpDir[1024];
      strcpy(tmpDir, "./");
      strcat(tmpDir, dirStruct -> d_name);

      strcpy(sfp, showFilePath);//
      strcat(sfp, dirStruct -> d_name);
      strcat(sfp, "/");

      recursiveCatalogSearch(tmpDir, sfp);
      chdir("..");
    }
  }

  closedir(dirp);
}

int main(int argc, char * argv[]) {
  char bufPath[1024];
  strcpy(bufPath, "."); //Here will be 1. argument instead of "."
  // strcpy(bufPath, "/home/michal/Desktop/Semestr 4/Systemy Operacyjne/OperatingSystems/laboratories2/exercise2");

  recursiveCatalogSearch("./", "./");
}


void describeFile(struct stat buffer, char * path) {
  printf("%7.ld ", buffer.st_size);
  printf("%s ", getMode(buffer.st_mode));
  char date[20];
  printf("%s ", formatDate(date, buffer.st_mtime));
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
  strftime(str, 20, "%d.%m.%Y %H:%M:%S", localtime(&val));
  return str;
}
