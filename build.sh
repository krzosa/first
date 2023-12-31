#!/usr/bin/env bash
set -e

mkdir build
cd build
clang -o test_array ../test/test_array.cpp -fno-exceptions -fno-rtti -Wno-writable-strings
./test_array
cd ..