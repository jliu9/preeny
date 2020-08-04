#include "utest.h"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

const char *g_fsp_test_root_dir = "fsp/";
int g_pass = 1;

void print_pass() {
  if (g_pass) {
    fprintf(stdout, "PASS\n");
  } else {
    fprintf(stdout, "FAIL\n");
  }
}

int walk_test_root_dir() {
  char root_dir_name[100] = "";
  strcat(root_dir_name, g_fsp_test_root_dir);
  DIR *dirp = opendir(root_dir_name);
  struct dirent *dp;
  if (dirp == NULL) {
    fprintf(stderr, "cannot open test_root_dir:%s\n", root_dir_name);
    return -1;
  }
  for (;;) {
    dp = readdir(dirp);
    if (dp == NULL) break;
    fprintf(stdout, "ino:%lu fname:%s\n", dp->d_ino, dp->d_name);
  }
  closedir(dirp);
  return 0;
}

void print_sep_line() { fprintf(stdout, "=======\n"); }
