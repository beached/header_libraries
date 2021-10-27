@echo off
cd build

ECHO "#################################3"
ECHO "Starting Tests"
"C:/PROGRAM FILES (X86)/MICROSOFT VISUAL STUDIO/2019/ENTERPRISE/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/ctest.exe" -C Debug --target daw-header-libraries_full -j 2 -VV --timeout 300
