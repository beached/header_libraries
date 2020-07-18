@ECHO OFF

ECHO "##############################"
ECHO "Installing Ninja"
vcpkg upgrade
vcpkg install ninja
choco install llvm

md build
cd build

ECHO "##############################"
ECHO "Setting VCVars"
@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64

ECHO "##############################"
ECHO "Running cmake"
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja -DCMAKE_CXX_COMPILER=clang-cl.exe -DCMAKE_C_COMPILER=clang-cl.exe ..

ECHO "##############################"
ECHO "Building"
cmake --build . --config Debug --target full -j 2 -- -k 1000
