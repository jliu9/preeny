#!/bin/sh
export PREENY_DEBUG=1
export PREENY_INFO=1
export PREENY_ERROR=1
export FSP_KEY_LISTS="1"

# unlink
LD_PRELOAD=lib/libfsp.so ./bin/test_unlink1
