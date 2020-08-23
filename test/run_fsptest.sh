#!/bin/sh
export PREENY_DEBUG=1
export PREENY_INFO=1
export PREENY_ERROR=1
#export FSP_KEY_LISTS="1,11"

# unlink
#LD_PRELOAD=lib/libfsp.so ./bin/test_unlink1
#LD_PRELOAD=lib/libfsp.so ./bin/test_unlink2
#LD_PRELOAD=lib/libfsp.so ./bin/test_unlink3

TC_NAME="$1"
KEY_NUM="$2"

BIN_DIR="./bin/"
TEST_DIR="./bin/fsp/test_dir/"

if [ $KEY_NUM -lt 2 ]; then
    EXPORT FSP_KEY_LISTS="1"
else
    EXPORT FSP_KEY_LISTS="1,11"
fi

if [ -f "${BIN_DIR}/${TC_NAME}" ]; then
    echo "======= ${TC_NAME} ======="
else
    echo "${BIN_DIR}/${TC_NAME} not exists"
    exit
fi

mkdir -p "${TEST_DIR}"

LD_PRELOAD=lib/libfsp.so ./bin/test_"$TC_NAME"
