#!/bin/sh
export PREENY_DEBUG=1
export PREENY_INFO=1
export PREENY_ERROR=1
#export FSP_KEY_LISTS="1,11"


KEY_NUM="$1"
DIR_NAME="$2"


if [ $# -ne 2 ]; then
    echo "Usage: run_fstest.sh <KEY_NUM> <DIR_NAME>"
    exit
fi


if [ "$KEY_NUM" -lt 2 ]; then
    export FSP_KEY_LISTS="1"
else
    export FSP_KEY_LISTS="1,11"
fi

rm -rf -p "dcpdir_out"

LD_PRELOAD=lib/libfsp.so ./bin/dcpdir ${DIR_NAME}
