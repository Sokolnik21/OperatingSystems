#include <dirent.h>
#include "CatalogOperations.h"

#include <string.h>
#include <stdio.h>

// For chdir()
#include <unistd.h>


int main(int argc, char * argv) {
  DIR * dirp;
  dirp = opendir(".");

  struct dirent * dirStruct;
  dirStruct = readdir(dirp);

  while((dirStruct = readdir(dirp)) != NULL) {
    if(dirStruct -> d_type == DT_REG) {
      printf("Regular file: %s\n", dirStruct -> d_name);
    }
    if(dirStruct -> d_type == DT_DIR) {
      printf("Catalog: %s\n", dirStruct -> d_name);
    }
  }

  closedir(dirp);
}
