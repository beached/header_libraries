@echo off
choco install ninja
md build
cd build
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\cvarsall.bat" x64
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja ..
cmake --build . --config Debug --target full -j 2 -- -k 1000
