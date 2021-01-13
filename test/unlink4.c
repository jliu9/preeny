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

// Test unlink can return the data block correctly
// write a large amount of data

extern char *g_fsp_test_root_dir;
extern int g_pass;

void test_write_unlink1() {
  // variables
  char *test_dir_name = "fsp/test_dir";
  char *test_f_name = "fsp/test_dir/f";
  int rt, fd;
  struct stat stbuf;

  // setup
  rt = mkdir(test_dir_name, 0770);
  CHECK_ACTUAL_EXPECT_VALUE(mkdir, rt, 0);

  // create files
  const int kBufSize = 4096;
  const int kNumFiles = 200;
  const uint64_t kFbytes = ((uint64_t)1024) * 100 * 1024;
  // const uint64_t kFbytes = ((uint64_t)1024) * 12;
  // const uint64_t kFbytes = ((uint64_t)1024) * 16;
  char test_text[kBufSize];
  char cur_fname[128];
  memset(test_text, 'A', kBufSize);
  for (int fi = 0; fi < kNumFiles; fi++) {
    memset(cur_fname, 0, 128);
    sprintf(cur_fname, "fsp/test_dir/f%d", fi);
    fd = open(cur_fname, O_CREAT | O_RDWR, 0770);
    CHECK_COND(creatf, fd > 0);
    for (uint64_t i = 0; i < kFbytes / kBufSize; i++) {
      rt = write(fd, test_text, kBufSize);
      CHECK_ACTUAL_EXPECT_VALUE(writef, rt, kBufSize);
    }
    rt = close(fd);
    CHECK_ACTUAL_EXPECT_VALUE(closef, rt, 0);
    assert(g_pass);

    rt = unlink(cur_fname);
    CHECK_ACTUAL_EXPECT_VALUE(unlinkf, rt, 0);
    assert(g_pass);
    rt = stat(cur_fname, &stbuf);
    CHECK_ACTUAL_EXPECT_VALUE(statunlinkedf1, rt, -1);
    assert(g_pass);
  }
}

void test_write_unlink2() {
  // variables
  char *test_dir_name = "fsp/test_dir";
  char *test_f_name = "fsp/test_dir/f";
  int rt, fd;
  struct stat stbuf;

  // setup
  rt = stat(test_dir_name, &stbuf);
  if (rt < 0) {
    rt = mkdir(test_dir_name, 0770);
    CHECK_ACTUAL_EXPECT_VALUE(mkdir, rt, 0);
  }

  // create files
  const int kBufSize = 4096;
  const int kNumFiles = 100;
  const uint64_t kFbytes = ((uint64_t)1024) * 100 * 1024;
  // const uint64_t kFbytes = ((uint64_t)1024) * 12;
  // const uint64_t kFbytes = ((uint64_t)1024) * 16;
  char test_text[kBufSize];
  char cur_fname[128];
  memset(test_text, 'A', kBufSize);
  for (int fi = 0; fi < kNumFiles; fi++) {
    memset(cur_fname, 0, 128);
    sprintf(cur_fname, "fsp/test_dir/f%d", fi);
    fd = open(cur_fname, O_CREAT | O_RDWR, 0770);
    CHECK_COND(creatf, fd > 0);
    for (uint64_t i = 0; i < kFbytes / kBufSize; i++) {
      rt = write(fd, test_text, kBufSize);
      CHECK_ACTUAL_EXPECT_VALUE(writef, rt, kBufSize);
    }
    rt = close(fd);
    CHECK_ACTUAL_EXPECT_VALUE(closef, rt, 0);
    assert(g_pass);
  }

  for (int fi = 0; fi < kNumFiles; fi++) {
    sprintf(cur_fname, "fsp/test_dir/f%d", fi);
    rt = unlink(cur_fname);
    CHECK_ACTUAL_EXPECT_VALUE(unlinkf, rt, 0);
    assert(g_pass);
    rt = stat(cur_fname, &stbuf);
    CHECK_ACTUAL_EXPECT_VALUE(statunlinkedf1, rt, -1);
    assert(g_pass);
  }
}

int main() {
  if (walk_test_root_dir() != 0) {
    return -1;
  }
  print_sep_line();
  // test_write_unlink1();
  // print_sep_line();
  test_write_unlink2();
  print_pass();
  print_sep_line();
  return 0;
}