#!/bin/bash

cd build

echo "#################################3"
echo "Starting Tests"
ctest -C Debug --target daw-header-libraries_full -j 2
