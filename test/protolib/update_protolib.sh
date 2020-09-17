#!/bin/sh

LIB_PATH=$1

########################################################################################################################
## Cleanup

rm -rf $LIB_PATH/src/
rm -rf $LIB_PATH/build/

########################################################################################################################
## Generate source files

mkdir -p $LIB_PATH/src/

protoc -I $LIB_PATH/proto/ --cpp_out=$LIB_PATH/src/ --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $LIB_PATH/proto/test.proto

########################################################################################################################
## Build static library

mkdir -p $LIB_PATH/build/

cd $LIB_PATH/build/

cmake -DCMAKE_BUILD_TYPE=Release ..
make
