#!/usr/bin/env bash
set -e

mkdir build
cd build
clang -o test_arena ../test/test_arena.cpp -fno-exceptions -fno-rtti -Wno-writable-strings
./test_arena
cd ..