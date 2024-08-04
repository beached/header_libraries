#!/bin/bash

cd build

echo "#################################"
echo "Starting Tests"
ctest -C Debug --build-target daw-header-libraries_full -j 2
