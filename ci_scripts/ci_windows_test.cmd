@echo off
cd build

ECHO "#################################3"
ECHO "Starting Tests"
ctest -C Debug --target full -j 2 -VV --timeout 300
