#!/bin/bash

mkdir build
cd build

echo "#################################"
echo "Configure Cmake"
CC=clang-9 CXX=clang++-9 CXXFLAGS=-stdlib=libc++ cmake -DCMAKE_BUILD_TYPE=Debug -DDAW_ALLOW_TESTING=On ..

echo "#################################"
echo "Starting full build"
cmake --build . --config Debug --target full -j 2 -- -k 1000
echo "#################################"
echo "Full build complete"

