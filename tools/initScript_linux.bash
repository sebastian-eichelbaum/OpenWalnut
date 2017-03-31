#!/bin/bash
mkdir build/release
cd build/release
cmake  -D CMAKE_BUILD_TYPE:STRING=Release ../../src
cmake . && make && make test && make stylecheck && make doc
cd ../..

mkdir build/debug
cd build/debug
cmake  -D CMAKE_BUILD_TYPE:STRING=Debug ../../src
cmake . && make && make test && make stylecheck && make doc
cd ../..
