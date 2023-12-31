#!/usr/bin/env bash
set -e

mkdir build
cd build
clang -o first ../test/test_arena.cpp -Wno-writable-strings
./first
cd ..