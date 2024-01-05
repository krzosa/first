#!/usr/bin/env bash
mkdir build
cd build
set -e
gcc -o bld ../bld_main.cpp -fno-exceptions -fno-rtti -Wno-write-strings
cd ..
./build/bld
# gcc -o test_filesystem ../test/test_filesystem.c -Wno-write-strings
# ./test_filesystem
# gcc -o test_array ../test/test_array.cpp -fno-exceptions -fno-rtti -Wno-write-strings
# ./test_array
# gcc -o test_table ../test/test_table.cpp -fno-exceptions -fno-rtti -Wno-write-strings
# ./test_table
# gcc -o compile_as_c ../test/main.c
# ./compile_as_c

#-Wno-writable-strings
cd ..