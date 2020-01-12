md build
cd build

REM ##############################
REM Running cmake
cmake ..
REM ##############################
REM Building
cmake --build . --config Debug --target full -j 2
