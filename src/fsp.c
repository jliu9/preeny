#define _GNU_SOURCE

#include <dirent.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "logging.h"

#include "fsapi.h"

#define FS_SHM_KEY_BASE 20190301
#define FS_MAX_NUM_WORKER 20

const static char *fsp_dir_prefix = "fsp";
// 3 == strlen(fsp_dir_prefix)
#define TO_NEW_PATH(path) (path + 3)

const static char *FSP_ENV_VAR_KEY_LIST = "FSP_KEY_LISTS";
static key_t shm_keys[FS_MAX_NUM_WORKER];
static int g_num_workers = 0;

/**
 * NOTE: it's better to use ltrace to see what libc is doing for this wrapper
 *
 int stat(const char *pathname, struct stat *statbuf);
 int open(const char *pathname, int flags);
 int open(const char *pathname, int flags, mode_t mode);
 int close(int fd);
 int unlink(const char *pathname);
 int rename(const char *oldpath, const char *newpath);
 int mkdir(const char *pathname, mode_t mode);
 int rmdir(const char *pathname);
 DIR *opendir(const char *name);
 int closedir(DIR *dirp);
 struct dirent *readdir(DIR *dirp);
 ssize_t read(int fd, void *buf, size_t count);
 ssize_t pread(int fd, void *buf, size_t count, off_t offset);
 ssize_t write(int fd, const void *buf, size_t count);
 ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
 **/

static void init_shm_keys(char *keys);
static void clean_exit() { exit(fs_exit()); };
static inline int check_if_path_fsp_data(const char *path) {
  return strncmp(path, fsp_dir_prefix, 3) == 0;
}

//
// originals
//
int (*original_stat)(const char *pathname, struct stat *statbuf);
int (*original__xstat)(int ver, const char *path, struct stat *stat_buf);
int (*original_open)(const char *pathname, int flags, mode_t mode);
int (*original_close)(int fd);
int (*original_unlink)(const char *pathname);
int (*original_rename)(const char *oldpath, const char *newpath);
int (*original_mkdir)(const char *pathname, mode_t mode);
int (*original_rmdir)(const char *pathname);
DIR *(*original_opendir)(const char *name);
int (*original_closedir)(DIR *dirp);
struct dirent *(*original_readdir)(DIR *dirp);
ssize_t (*original_read)(int fd, void *buf, size_t count);
ssize_t (*original_pread)(int fd, void *buf, size_t count, off_t offset);
ssize_t (*original_write)(int fd, const void *buf, size_t count);
ssize_t (*original_pwrite)(int fd, const void *buf, size_t count, off_t offset);
__attribute__((constructor)) void preeny_fsops_orig() {
  original_stat = dlsym(RTLD_NEXT, "stat");
  original__xstat = dlsym(RTLD_NEXT, "__xstat");
  original_open = dlsym(RTLD_NEXT, "open");
  original_close = dlsym(RTLD_NEXT, "close");
  original_unlink = dlsym(RTLD_NEXT, "unlink");
  original_rename = dlsym(RTLD_NEXT, "rename");
  original_mkdir = dlsym(RTLD_NEXT, "mkdir");
  original_opendir = dlsym(RTLD_NEXT, "opendir");
  original_closedir = dlsym(RTLD_NEXT, "closedir");
  original_read = dlsym(RTLD_NEXT, "read");
  original_pread = dlsym(RTLD_NEXT, "pread");
  original_write = dlsym(RTLD_NEXT, "write");
  original_pwrite = dlsym(RTLD_NEXT, "pwrite");
  char *fsp_num_workers_str = getenv(FSP_ENV_VAR_KEY_LIST);
  if (fsp_num_workers_str == NULL) {
    preeny_error(
        "cannot find environment variable: %s. Will use default single key\n",
        FSP_ENV_VAR_KEY_LIST);
    fsp_num_workers_str = "1";
  }
  init_shm_keys(fsp_num_workers_str);
  preeny_info("FSP:g_num_workers:%d\n", g_num_workers);

  int rt = fs_init_multi(g_num_workers, shm_keys);
  if (rt < 0) {
    preeny_error("cannot connect to FS\n");
  }
}

__attribute__((destructor)) void preeny_fsops_shutdown() { clean_exit(); }

int stat(const char *pathname, struct stat *statbuf) {
  preeny_debug("stat(%s)\n", pathname);
  int rt;
  if (check_if_path_fsp_data(pathname)) {
    rt = fs_stat(TO_NEW_PATH(pathname), statbuf);
  } else {
    rt = original_stat(pathname, statbuf);
  }
  return rt;
}

int __xstat(int ver, const char *path, struct stat *statbuf) {
  preeny_debug("__xstat(%s)\n", path);
  int rt;
  if (check_if_path_fsp_data(path)) {
    rt = fs_stat(TO_NEW_PATH(path), statbuf);
  } else {
    rt = original__xstat(ver, path, statbuf);
  }
  return rt;
}

int open(const char *pathname, int flags, mode_t mode) {
  preeny_debug("open(%s)\n", pathname);
  int fd;
  if (check_if_path_fsp_data(pathname)) {
    fd = fs_open(TO_NEW_PATH(pathname), flags, mode);
  } else {
    preeny_info("original_open(%s)\n", pathname);
    fd = original_open(pathname, flags, mode);
  }
  return fd;
}

int close(int fd) {
  preeny_debug("close\n");
  return fs_close(fd);
}
int unlink(const char *pathname) {
  preeny_debug("unlink\n");
  int rt;
  if (check_if_path_fsp_data(pathname)) {
    rt = fs_unlink(TO_NEW_PATH(pathname));
  } else {
    rt = unlink(pathname);
  }
  return rt;
}

int rename(const char *oldpath, const char *newpath) {
  preeny_debug("rename old:%s new:%s\n", oldpath, newpath);
  int rt;
  if (check_if_path_fsp_data(oldpath)) {
    rt = fs_rename(TO_NEW_PATH(oldpath), TO_NEW_PATH(newpath));
  } else {
    rt = rename(oldpath, newpath);
  }
  return rt;
}

int mkdir(const char *pathname, mode_t mode) {
  preeny_debug("mkdir\n");
  int rt;
  if (check_if_path_fsp_data(pathname)) {
    rt = fs_mkdir(TO_NEW_PATH(pathname), mode);
  } else {
    rt = mkdir(pathname, mode);
  }
  return rt;
}

int rmdir(const char *pathname) {
  preeny_debug("rmdir\n");
  int rt;
  if (check_if_path_fsp_data(pathname)) {
    rt = fs_rmdir(TO_NEW_PATH(pathname));
  } else {
    rt = original_rmdir(pathname);
  }
  return rt;
}

DIR *opendir(const char *name) {
  preeny_debug("opendir\n");
  if (check_if_path_fsp_data(name)) {
    return (DIR *)(fs_opendir(TO_NEW_PATH(name)));
  }
  return NULL;
}

int closedir(DIR *dirp) {
  preeny_debug("closedir\n");
  return fs_closedir((struct CFS_DIR *)dirp);
}

struct dirent *readdir(DIR *dirp) {
  preeny_debug("readdir\n");
  return fs_readdir((struct CFS_DIR *)dirp);
}

ssize_t read(int fd, void *buf, size_t count) {
  preeny_debug("read\n");
  return fs_read(fd, buf, count);
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
  preeny_debug("pread\n");
  return fs_pread(fd, buf, count, offset);
}

ssize_t write(int fd, const void *buf, size_t count) {
  preeny_debug("write\n");
  return fs_write(fd, buf, count);
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
  preeny_debug("pwrite\n");
  return fs_pwrite(fd, buf, count, offset);
}

static void init_shm_keys(char *keys) {
  preeny_debug("init_shm_keys:%s\n", keys);
  char *token = strtok(keys, ",");
  key_t cur_key;
  int num_workers = 0;
  while (token != NULL) {
    cur_key = atoi(token);
    preeny_debug("cur_key:%d\n", cur_key);
    shm_keys[num_workers++] = (FS_SHM_KEY_BASE) + cur_key;
    token = strtok(NULL, ",");
  }
  g_num_workers = num_workers;
}