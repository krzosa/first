#!/usr/bin/env bash
set -e

mkdir build
cd build
clang -o first ../test/main.cpp
./first
cd ..