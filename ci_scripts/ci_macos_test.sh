#!/bin/bash

cd build
ctest -C Debug --target full -j 2
