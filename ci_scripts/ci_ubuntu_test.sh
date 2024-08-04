#!/bin/bash

cd build

echo "#################################3"
echo "Starting Tests"
ctest -C Debug --build-target daw-header-libraries_full -j 2 -- -k 100
