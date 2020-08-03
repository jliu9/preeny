#define _GNU_SOURCE

#include <dirent.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "logging.h"

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

//
// originals
//
int (*original_stat)(const char *pathname, struct stat *statbuf);
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
}

int stat(const char *pathname, struct stat *statbuf) {
#ifdef KFS_ORIG
  return original_stat(pathname, statbuf);
#else
  return 0;
#endif
}

int open(const char *pathname, int flags, mode_t mode) {
#ifdef KFS_ORIG
  return original_open(pathname, flags, mode);
#else
  return 0;
#endif
}

int close(int fd) {
#ifdef KFS_ORIG
  return original_close(fd);
#else
  return 0;
#endif
}
int unlink(const char *pathname) {
#ifdef KFS_ORIG
  return original_unlink(pathname);
#else
  return 0;
#endif
}

int rename(const char *oldpath, const char *newpath) {
#ifdef KFS_ORIG
  return original_rename(oldpath, newpath);
#else
  return 0;
#endif
}

int mkdir(const char *pathname, mode_t mode) {
#ifdef KFS_ORIG
  return original_mkdir(pathname, mode);
#else
  return 0;
#endif
}

int rmdir(const char *pathname) {
#ifdef KFS_ORIG
  return original_rmdir(pathname);
#else
  return 0;
#endif
}

DIR *opendir(const char *name) {
#ifdef KFS_ORIG
  return original_opendir(name);
#else
  return 0;
#endif
}

int closedir(DIR *dirp) {
#ifdef KFS_ORIG
  return original_closedir(dirp);
#else
  return 0;
#endif
}

struct dirent *readdir(DIR *dirp) {
#ifdef KFS_ORIG
  return original_readdir(dirp);
#else
  return 0;
#endif
}

ssize_t read(int fd, void *buf, size_t count) {
#ifdef KFS_ORIG
  return original_read(fd, buf, count);
#else
  return 0;
#endif
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
#ifdef KFS_ORIG
  return original_pread(fd, buf, count, offset);
#else
  return 0;
#endif
}

ssize_t write(int fd, const void *buf, size_t count) {
#ifdef KFS_ORIG
  return original_write(fd, buf, count);
#else
  return 0;
#endif
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
#ifdef KFS_ORIG
  return original_pwrite(fd, buf, count, offset);
#else
  return 0;
#endif
}
