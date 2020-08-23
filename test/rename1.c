#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "utest.h"

extern char *g_fsp_test_root_dir;
extern int g_pass;

static const char *test_dir_name = "fsp/test_dir";
static const mode_t common_mode = 0770;

// src is FILE, dst does not exist
void test_rename1() {
  char *test_src_name = "fsp/test_dir/fsrc1";
  char *test_dst_name = "fsp/test_dir/fdst1";
  int rt, fd;
  struct stat stbuf;
  ino_t src_ino, dst_ino;

  fd = open(test_src_name, O_CREAT, common_mode);
  CHECK_COND(createfsrc, fd > 0);
  rt = close(fd);
  CHECK_ACTUAL_EXPECT_VALUE(closefsrc, rt, 0);
  assert(g_pass);

  rt = stat(test_src_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statfsrc, rt, 0);
  assert(g_pass);
  src_ino = stbuf.st_ino;

  rt = rename(test_src_name, test_dst_name);
  CHECK_ACTUAL_EXPECT_VALUE(rename1, rt, 0);
  assert(g_pass);

  rt = stat(test_src_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statfsrc, rt, -1);
  rt = stat(test_dst_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statfdst, rt, 0);
  dst_ino = stbuf.st_ino;
  CHECK_COND(inoTheSame, src_ino == dst_ino);
  assert(g_pass);
}

// src is DIR, dst does not exist
void test_rename2() {
  char *test_src_name = "fsp/test_dir/dsrc2";
  char *test_dst_name = "fsp/test_dir/ddst2";
  int rt;
  struct stat stbuf;
  ino_t src_ino, dst_ino;

  rt = mkdir(test_src_name, common_mode);
  CHECK_ACTUAL_EXPECT_VALUE(mkdsrc, rt, 0);
  assert(g_pass);

  rt = stat(test_src_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statdsrc, rt, 0);
  CHECK_COND(srcdirsize, stbuf.st_size > 0);
  assert(g_pass);
  src_ino = stbuf.st_ino;

  rt = rename(test_src_name, test_dst_name);
  CHECK_ACTUAL_EXPECT_VALUE(rename2, rt, 0);
  assert(g_pass);

  rt = stat(test_src_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statdsrc, rt, -1);
  rt = stat(test_dst_name, &stbuf);
  CHECK_ACTUAL_EXPECT_VALUE(statddst, rt, 0);
  dst_ino = stbuf.st_ino;
  CHECK_COND(inoTheSame, src_ino == dst_ino);
  assert(g_pass);
}

// src, dist are FILEs, both exists
void test_rename3() {}

// src, dst are DIRs, both exists
void test_rename4() {}

// void teardown() {
//   int rt = rmdir(test_dir_name);
//   CHECK_ACTUAL_EXPECT_VALUE(rmdir, rt, 0);
// }

int main() {
  if (walk_test_root_dir() != 0) {
    return -1;
  }
  print_sep_line();
  test_rename1();
  print_pass();
  print_sep_line();
  test_rename2();
  print_pass();
  print_sep_line();
  return 0;
}