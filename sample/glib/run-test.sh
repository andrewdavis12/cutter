#!/bin/sh

export BASE_DIR="`dirname $0`"

if test x"$NO_MAKE" != x"yes"; then
    make -C ../../ > /dev/null || exit 1
fi

CUT_OUTPUT_MODULE_DIR=$BASE_DIR/../../cutter/module/output/.libs $BASE_DIR/../../cutter/cutter \
    --color=auto --multi-thread -s $BASE_DIR "$@" $BASE_DIR
