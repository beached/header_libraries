#!/bin/bash

mkdir build
cd build

echo "#################################"
echo "Configure Cmake"
cmake -DCMAKE_BUILD_TYPE=Debug .. -DDAW_ENABLE_TESTING=On -DCMAKE_INSTALL_PREFIX=/tmp/header_libraries

echo "#################################"
echo "Starting full build"
cmake --build . --config Debug --target daw-header-libraries_full -j 2
echo "#################################"
echo "Full build complete"
