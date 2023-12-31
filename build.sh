#!/usr/bin/env bash

mkdir build
cd build
clang -o first ../test/main.cpp
./first
cd ..