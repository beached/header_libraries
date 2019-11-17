#!/bin/bash

mkdir build
cd build
CC=clang CXX=clang++ CXXFLAGS=-stdlib=libc++ cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug --target full -j 2
ctest -C Debug

