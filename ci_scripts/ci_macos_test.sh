#!/bin/bash

cd build

echo "#################################"
echo "Starting Tests"
ctest -C Debug --target full -j 2
