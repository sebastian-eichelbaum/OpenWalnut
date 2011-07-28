#!/bin/bash
mkdir build/release
cd build/release
cmake  -D CMAKE_BUILD_TYPE:STRING=Release ../../src
make many
cd ../..

mkdir build/debug
cd build/debug
cmake  -D CMAKE_BUILD_TYPE:STRING=Debug ../../src
make many
cd ../..
