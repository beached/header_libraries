@echo off
md build
cd build
cmake  ..
cmake --build . --config Debug --target full -j 2
ctest -C Debug
