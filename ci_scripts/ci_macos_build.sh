#!/bin/bash

mkdir build
cd build

echo "#################################"
echo "Configure Cmake"
cmake -DCMAKE_BUILD_TYPE=Debug .. -DDAW_ENABLE_TESTING=On

echo "#################################"
echo "Starting full build"
cmake --build . --config Debug --target daw-header-libraries_full -j 2 -- -k 1000
echo "#################################"
echo "Full build complete"
