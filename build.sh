#!/bin/bash -eu
HERE=$(cd $(dirname $0); pwd)
BUILD=$HERE/build
if [ ! -d $BUILD ]; then
    mkdir $BUILD
fi
cd $BUILD
cmake ../cmake
make -j
