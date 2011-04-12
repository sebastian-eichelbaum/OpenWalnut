#!/bin/bash
mkdir build/release
cd build/release
cmake ../../src
make many
cd ../..

mkdir build/debug
cd build/debug
cmake ../../src
make many
cd ../..
