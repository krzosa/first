#!/usr/bin/env bash
set -e

mkdir build
cd build
clang -o test_array ../test/test_array.cpp -fno-exceptions -fno-rtti -Wno-writable-strings
./test_array
clang -o test_table ../test/test_table.cpp -fno-exceptions -fno-rtti -Wno-writable-strings
./test_table
cd ..