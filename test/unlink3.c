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
// unlink can be successed for an opened worker, without affecting opened FD
// read/write

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
  const char *test_str = "helloworld";
  int test_str_len = (int)strlen(test_str);
  fd = open(test_f1_name, O_CREAT | O_RDWR, 0770);
  CHECK_COND(creatf1, fd > 0);
RETRY_WRITE1:
  rt = write(fd, test_str, strlen(test_str));
  if (rt == FSP_MT_RETRY_ERROR_NUM) {
    goto RETRY_WRITE1;
  }
  CHECK_ACTUAL_EXPECT_VALUE(writef1, rt, test_str_len);
  rt = close(fd);
  CHECK_ACTUAL_EXPECT_VALUE(closef1, rt, 0);
  assert(g_pass);
  fd = open(test_f2_name, O_CREAT | O_RDWR, 0770);
RETRY_WRITE2:
  rt = write(fd, test_str, strlen(test_str));
  if (rt == FSP_MT_RETRY_ERROR_NUM) {
    goto RETRY_WRITE2;
  }
  CHECK_ACTUAL_EXPECT_VALUE(writef2, rt, test_str_len);
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

  int fd1, fd2;
  // now we open files again
  fd = open(test_f1_name, O_RDONLY, 0770);
  CHECK_COND(openf1, fd > 0);
  fd1 = fd;
  fd = open(test_f2_name, O_RDONLY, 0770);
  CHECK_COND(openf2, fd > 0);
  fd2 = fd;
  assert(g_pass);

  rt = unlink(test_f1_name);
  CHECK_ACTUAL_EXPECT_VALUE(unlinkf1, rt, 0);
  assert(g_pass);
  rt = stat(test_f1_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statunlinkedf1, rt, -1);
  assert(g_pass);
  // read again
  char buf1[100];
RETRY_READ1:
  rt = pread(fd1, buf1, test_str_len, 0);
  if (rt == FSP_MT_RETRY_ERROR_NUM) {
    goto RETRY_READ1;
  }
  CHECK_ACTUAL_EXPECT_VALUE(readunlinked1, rt, test_str_len);
  assert(g_pass);

  rt = unlink(test_f2_name);
  CHECK_ACTUAL_EXPECT_VALUE(unlinkf2, rt, 0);
  assert(g_pass);
  rt = stat(test_f2_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statunlinkedf2, rt, -1);
  assert(g_pass);
  // read again
  char buf2[100];
RETRY_READ2:
  rt = pread(fd1, buf2, test_str_len, 0);
  if (rt == FSP_MT_RETRY_ERROR_NUM) {
    goto RETRY_READ2;
  }
  CHECK_ACTUAL_EXPECT_VALUE(readunlinked2, rt, test_str_len);
  assert(g_pass);
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