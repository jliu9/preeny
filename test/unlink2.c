#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utest.h"

// This will use 2 files for testing
// when use > 1 worker, two files are supposed to have one file that is
// in another worker
// NOTE: adjust the splitting policy to make this work...
// The goal of this test case is to testing:
// Once one file is moved to another worker, if the unlink works or not

extern char *g_fsp_test_root_dir;
extern int g_pass;

void test_unlink_1() {
  // variables
  char *test_dir_name = "fsp/test_dir";
  char *test_f1_name = "fsp/test_dir/f1";
  char *test_f2_name = "fsp/test_dir/f2";
  int rt, fd;
  struct stat stbuf;

  // setup
  rt = mkdir(test_dir_name, 0770);
  CHECK_ACTUAL_EXPECT_VALUE(mkdir, rt, 0);

  // run tests
  rt = stat(test_f1_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statf1, rt, -1);
  rt = stat(test_f2_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statf2, rt, -1);

  // create files
  fd = open(test_f1_name, O_CREAT, 0770);
  CHECK_COND(creatf1, fd > 0);
  rt = close(fd);
  CHECK_ACTUAL_EXPECT_VALUE(closef1, rt, 0);
  assert(g_pass);
  fd = open(test_f2_name, O_CREAT, 0770);
  CHECK_COND(createf2, fd > 0);
  rt = close(fd);
  CHECK_ACTUAL_EXPECT_VALUE(closef2, rt, 0);
  assert(g_pass);

  // move? wow
  for (int i = 0; i < 2; i++) {
    rt = stat(test_f1_name, &stbuf);
    CHECK_ACTUAL_EXPECT_VALUE(statf1, rt, 0);
    rt = stat(test_f2_name, &stbuf);
    CHECK_ACTUAL_EXPECT_VALUE(statf2, rt, 0);
    assert(g_pass);
  }

  rt = unlink(test_f1_name);
  CHECK_ACTUAL_EXPECT_VALUE(unlinkf1, rt, 0);
  assert(g_pass);
  rt = stat(test_f1_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statunlinkedf1, rt, -1);
  assert(g_pass);
  rt = unlink(test_f1_name);
  CHECK_ACTUAL_EXPECT_VALUE(unlinkagainf1, rt, -1);
  assert(g_pass);
  fd = open(test_f1_name, O_CREAT, 0770);
  CHECK_COND(recreatef1, fd > 0);
  rt = close(fd);
  CHECK_ACTUAL_EXPECT_VALUE(closef1, rt, 0);

  rt = unlink(test_f2_name);
  CHECK_ACTUAL_EXPECT_VALUE(unlinkf2, rt, 0);
  assert(g_pass);
  rt = stat(test_f2_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statunlinkedf2, rt, -1);
  assert(g_pass);
  rt = unlink(test_f2_name);
  CHECK_ACTUAL_EXPECT_VALUE(unlinkagainf2, rt, -1);
  assert(g_pass);
  fd = open(test_f2_name, O_CREAT, 0770);
  CHECK_COND(recreatef2, fd > 0);
  rt = close(fd);
  CHECK_ACTUAL_EXPECT_VALUE(closef2, rt, 0)
}

int main() {
  if (walk_test_root_dir() != 0) {
    return -1;
  }
  print_sep_line();
  test_unlink_1();
  print_pass();
  print_sep_line();
  return 0;
}