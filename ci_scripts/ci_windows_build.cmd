@echo off
choco install ninja
md build
cd build
cvarsall.bat x64
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja ..
cmake --build . --config Debug --target full -j 2 -- -k 1000
