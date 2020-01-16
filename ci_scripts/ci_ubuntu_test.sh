#!/bin/bash

cd build

echo "#################################3"
echo "Starting Tests"
ctest -C Debug --target full -j 2
