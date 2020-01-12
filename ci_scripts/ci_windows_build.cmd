REM ##############################
REM Installing Ninja
choco install ninja
md build
cd build

REM ##############################
REM Setting VCVars
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
REM ##############################
REM Running cmake
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja ..
REM ##############################
REM Building
cmake --build . --config Debug --target full -j 2 -- -k 1000
