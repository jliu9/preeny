#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utest.h"

/**
 * copy all files in one single directory to the running directory
 */

void print_usage(char **argv) {
  fprintf(stderr, "Usage: %s <dirname> [target_str]\n", argv[0]);
}

int stupid_grep(const char *dir_name, const char *target) {
  int rt;
  char fsp_dir_name[512] = "fsp/";
  char output_dir_name[512] = "dcpdir_out/";
  struct stat stbuf;
  strcat(fsp_dir_name, dir_name);
  // strcat(output_dir_name, dir_name);

  if (stat(output_dir_name, &stbuf) < 0) {
    rt = mkdir(output_dir_name, 0770);
    fprintf(stderr, "ERROR:%s\n", strerror(errno));
    assert(rt == 0);
  }

  DIR *dirp = opendir(fsp_dir_name);
  struct dirent *dp;
  if (dirp == NULL) {
    fprintf(stderr, "cannot open dir:%s\n", dir_name);
    return -1;
  }
  char fsp_file_name[512] = "";
  char output_file_name[512] = "";
  for (;;) {
    dp = readdir(dirp);
    if (dp == NULL) break;
    fprintf(stdout, "ino:%lu fname:%s\n", dp->d_ino, dp->d_name);
    if (strcmp(dp->d_name, ".") == 0) continue;
    if (strcmp(dp->d_name, "..") == 0) continue;
    sprintf(fsp_file_name, "%s/%s", fsp_dir_name, dp->d_name);
    sprintf(output_file_name, "%s/%s", output_dir_name, dp->d_name);
    fprintf(stdout, "will read %s\n", fsp_file_name);
    int kfd = open(output_file_name, O_CREAT | O_RDWR, 0770);
    rt = stat(fsp_file_name, &stbuf);
    assert(rt >= 0);
    size_t fsize = stbuf.st_size;
    int ffd = open(fsp_file_name, O_RDONLY);
    const int kReadSz = 4096;
    char readBuf[kReadSz];
    ssize_t rsize = 0;
    while (1) {
      rt = read(ffd, readBuf, kReadSz);
      if (rt >= 0) {
        write(kfd, readBuf, rt);
        rsize += rt;
      }
      if (rt != kReadSz) {
        fsync(kfd);
        break;
      }
    }
    if (rsize != fsize) {
      fprintf(stderr, "rsize:%ld fsize:%ld\n", rsize, fsize);
      assert(0);
    }
  }
  closedir(dirp);
  return rt;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage(argv);
    exit(1);
  }
  char *target_str = NULL;
  if (argc == 3) {
    target_str = argv[2];
  }
  stupid_grep(argv[1], argv[2]);
  return 0;
}