#pragma once
#include <stdio.h>


#define CHECK_COND(val, cond)                                      \
  if (!(cond)) {                                                   \
    g_pass = 0;                                                     \
    fprintf(stderr, "%s: %s not satisfied. err:%s\n", #val, #cond, \
            strerror(errno));                                      \
  }

#define CHECK_ACTUAL_EXPECT_VALUE(val, a, e)                        \
  if (a != e) {                                                     \
    g_pass = 0;                                                      \
    fprintf(stderr, "%s actual:%d expect:%d err: %s\n", #val, a, e, \
            strerror(errno));                                       \
  }

void print_sep_line();
int walk_test_root_dir();
void print_pass();
