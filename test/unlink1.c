

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static const char *fsp_test_root_dir = "fsp/";

void walk_test_root_dir() {
  char root_dir_name[100] = "";
  strcat(root_dir_name, fsp_test_root_dir);
  DIR *dirp = opendir(root_dir_name);
  struct dirent *dp;
  if (dirp == NULL) {
    fprintf(stderr, "cannot open test_root_dir:%s\n", root_dir_name);
    exit(1);
  }
  for (;;) {
    dp = readdir(dirp);
    if (dp == NULL) break;
    fprintf(stdout, "ino:%lu fname:%s\n", dp->d_ino, dp->d_name);
  }
  closedir(dirp);
}

int main() {
  walk_test_root_dir();
  return 0;
}