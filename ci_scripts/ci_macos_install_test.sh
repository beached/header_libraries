#!/bin/bash

set -e

cd build

echo "#################################"
echo "Starting Install Tests"
cmake --install .
cd ../cmake/test_project/
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=/tmp/header_libraries/
cmake --build .
ctest -C Debug -VV
