@ECHO OFF

ECHO "##############################"
ECHO "Installing Ninja"
vcpkg install ninja

md build
cd build

ECHO "##############################"
ECHO "Setting VCVars"
@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64

ECHO "##############################"
ECHO "Running cmake"
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja ..

ECHO "##############################"
ECHO "Building"
cmake --build . --config Debug --target full -j 2 -- -k 1000
