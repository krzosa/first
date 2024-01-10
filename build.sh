#!/usr/bin/env bash
gcc -o bld code/build_main.cpp -g
./bld cc=clang
