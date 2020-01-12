@echo off
md build
cd build
cmake  -G "Ninja" ..
cmake --build . --config Debug --target full -j 2 -- -k 1000
