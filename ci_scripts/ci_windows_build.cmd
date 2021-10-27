@ECHO OFF

ECHO "##############################"
ECHO "Installing Ninja"
vcpkg upgrade
REM vcpkg install ninja

md build
cd build

ECHO "##############################"
ECHO "Setting VCVars"
@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64

ECHO "##############################"
ECHO "Running cmake"
"C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2019/ENTERPRISE/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/cmake.exe" -DCMAKE_BUILD_TYPE=Debug -GNinja -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_C_COMPILER=cl.exe -DDAW_ENABLE_TESTING=On ..


ECHO "##############################"
ECHO "Building"
"C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2019/ENTERPRISE/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/cmake.exe" --build . --config Debug --target daw-header-libraries_full -j 2 -- -k 1000
