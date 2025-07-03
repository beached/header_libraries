@echo off
cd build


ECHO "##############################"
ECHO "Setting VCVars"
@call "C:\Program Files (x86)\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64

echo "#################################"
echo "Starting Install Tests"
cmake --install .
cd ../cmake/test_project/
md build
cd build
"C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2022/ENTERPRISE/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/cmake.exe" -DCMAKE_BUILD_TYPE=Debug -GNinja -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_C_COMPILER=cl.exe ..
IF %ERRORLEVEL% NEQ 0 EXIT %ERRORLEVEL%
"C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2022/ENTERPRISE/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/cmake.exe" --build . --config Debug
IF %ERRORLEVEL% NEQ 0 EXIT %ERRORLEVEL%
"C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2022/ENTERPRISE/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/ctest.exe" -C Debug -VV
IF %ERRORLEVEL% NEQ 0 EXIT %ERRORLEVEL%
