cmake_minimum_required (VERSION 2.6)
project (Preeny)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(common-preeny STATIC src/logging.c)

# Malloc
add_library(crazyrealloc SHARED src/crazyrealloc.c)
target_link_libraries(crazyrealloc common-preeny dl)
add_library(mallocwatch SHARED src/mallocwatch.c)
target_link_libraries(mallocwatch common-preeny dl)

# FSP
set(CFS_ROOT_DIR "$ENV{HOME}/workspace/ApparateFS/cfs/")
message("CFS_ROOT_DIR:${CFS_ROOT_DIR}")
set(CFS_INC "${CFS_ROOT_DIR}/include")
set(CFS_BUILD "${CFS_ROOT_DIR}/build")

include_directories(${CFS_INC})
message("CFS_INC:${CFS_INC}")
link_directories(${CFS_BUILD})

find_package(gflags REQUIRED)
set(GFLAGS_INCLUDE_DIR /usr/lib/x86_64-linux-gnu)
include_directories(${gflags_INCLUDE_DIR})

execute_process(
  COMMAND uname -r
  COMMAND sed "s/-.*$//"
  RESULT_VARIABLE tbb_kver_res
  OUTPUT_VARIABLE tbb_kver_out
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if(NOT ${tbb_kver_res} EQUAL "0")
  message(FATAL_ERROR "failed to get kernel version")
endif()

link_directories(
  "${CFS_ROOT_DIR}/lib/tbb/build/linux_intel64_gcc_cc7_libc2.27_kernel${tbb_kver_out}_release"
)

add_library(fsp SHARED src/fsp.c)
#target_compile_definitions(fsp PRIVATE KFS_ORIG)
target_link_libraries(fsp common-preeny dl cfs rt stdc++ tbb iberty)

# Signals
add_library(dealarm SHARED src/dealarm.c)
target_link_libraries(dealarm common-preeny)
add_library(desigact SHARED src/desigact.c)
target_link_libraries(desigact common-preeny)
add_library(startstop SHARED src/startstop.c)
target_link_libraries(startstop common-preeny)

# Defork
add_library(defork SHARED src/defork.c)
target_link_libraries(defork common-preeny)

# Deptrace
add_library(deptrace SHARED src/deptrace.c)
target_link_libraries(deptrace common-preeny)

# Sleep
add_library(desleep SHARED src/desleep.c)
target_link_libraries(desleep common-preeny)

# Socket
add_library(desock SHARED src/desock.c)
target_link_libraries(desock common-preeny dl pthread)
add_library(desock_dup SHARED src/desock_dup.c)
target_link_libraries(desock_dup common-preeny dl)
add_library(ensock SHARED src/ensock.c)
target_link_libraries(ensock common-preeny dl)
add_library(writeout SHARED src/writeout.c)
target_link_libraries(writeout common-preeny dl)

# Random
add_library(desrand SHARED src/desrand.c)
target_link_libraries(desrand common-preeny dl)
add_library(derand SHARED src/derand.c)
target_link_libraries(derand common-preeny dl)

# Time
add_library(detime SHARED src/detime.c)
target_link_libraries(detime common-preeny dl)

# UID
add_library(deuid SHARED src/deuid.c)
target_link_libraries(deuid common-preeny dl)

# Patch
add_library(patch SHARED src/patch.c)
target_link_libraries(patch common-preeny ini_config dl)

# EOF killer
add_library(eofkiller SHARED src/eofkiller.c)
target_link_libraries(eofkiller common-preeny dl)

# Canary
add_library(getcanary SHARED src/getcanary.c)
target_link_libraries(getcanary common-preeny dl)
add_library(setcanary SHARED src/setcanary.c)
target_link_libraries(setcanary common-preeny dl)

# Tests
add_executable(test_hello test/hello.c)
add_executable(test_rand test/rand.c)
add_executable(test_realloc test/realloc.c)
add_executable(test_sleep test/sleep.c)
add_executable(test_sock test/sock.c)
add_executable(test_uid test/uid.c)
set(FSP_TEST_COMMON_SRC test/fsptest_common.c test/utest.h)
add_executable(test_unlink1 test/unlink1.c ${FSP_TEST_COMMON_SRC})
add_executable(test_unlink2 test/unlink2.c ${FSP_TEST_COMMON_SRC})
add_executable(test_unlink3 test/unlink3.c ${FSP_TEST_COMMON_SRC})
add_executable(test_unlink4 test/unlink4.c ${FSP_TEST_COMMON_SRC})
add_executable(test_rename1 test/rename1.c ${FSP_TEST_COMMON_SRC})
add_executable(test_rename2 test/rename2.c ${FSP_TEST_COMMON_SRC})
add_executable(dcpdir test/deepcopydir.c ${FSP_TEST_COMMON_SRC})

# Scripts
file(COPY ${CMAKE_SOURCE_DIR}/test/run_tests.sh DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/test/run_fsptest.sh DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/test/run_dcpdir.sh DESTINATION ${CMAKE_BINARY_DIR})
