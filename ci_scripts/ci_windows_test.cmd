@echo off
cd build
ctest -C Debug --target full -j 2 -VV
