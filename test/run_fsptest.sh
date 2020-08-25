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

if [ $# -ne 2 ]; then
    echo "Usage: run_fstest.sh <TC_NAME> <KEY_NUM>"
    exit
fi


if [ "$KEY_NUM" -lt 2 ]; then
    export FSP_KEY_LISTS="1"
else
    export FSP_KEY_LISTS="1,11"
fi

if [ -f "${BIN_DIR}/test_${TC_NAME}" ]; then
    echo "======= ${TC_NAME} ======="
else
    echo "${BIN_DIR}/test_${TC_NAME} not exists"
    exit
fi

LD_PRELOAD=lib/libfsp.so ./bin/test_"$TC_NAME"
