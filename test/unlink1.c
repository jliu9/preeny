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

extern char *g_fsp_test_root_dir;
extern int g_pass;

// test case from:
// https://github.com/jliu9/ltp/blob/orig/testcases/kernel/syscalls/unlink/unlink05.c

void test_ulink_0() {
  // variables
  char *test_dir_name = "fsp/test_dir";
  char *test_f1_name = "fsp/test_dir/f1";
  char *test_f2_name = "fsp/test_dir/f2";
  int rt, fd;
  struct stat stbuf;

  // setup
  rt = mkdir(test_dir_name, 0770);
  CHECK_ACTUAL_EXPECT_VALUE(mkdir, rt, 0);

  rt = stat(test_f1_name, &stbuf);
  // CHECK_ACTUAL_EXPECT_VALUE(statf1, rt, -1);
  CHECK_COND(statf1, rt < 0);

  fd = open(test_f1_name, O_CREAT, 0770);
  CHECK_COND(creatf1, fd > 0);
  rt = close(fd);
  CHECK_ACTUAL_EXPECT_VALUE(closef1, rt, 0);
  assert(g_pass);

  rt = stat(test_f1_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statf1, rt, 0);
  assert(g_pass);

  rt = unlink(test_f1_name);
  CHECK_ACTUAL_EXPECT_VALUE(unlinkf1, rt, 0);
  assert(g_pass);

  rt = stat(test_f1_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statunlinked, rt, -1);
  assert(g_pass);

  rt = unlink(test_f1_name);
  CHECK_ACTUAL_EXPECT_VALUE(unlinkagain, rt, -1);
  assert(g_pass);

  rt = unlink(test_f2_name);
  CHECK_ACTUAL_EXPECT_VALUE(unlinknoexist, rt, -1);
  assert(g_pass);

  // tear_down
  rt = rmdir(test_dir_name);
  CHECK_ACTUAL_EXPECT_VALUE(rmdir, rt, 0);
}

int main() {
  if (walk_test_root_dir() != 0) {
    return -1;
  }
  print_sep_line();
  test_ulink_0();
  print_pass();
  print_sep_line();
  return 0;
}