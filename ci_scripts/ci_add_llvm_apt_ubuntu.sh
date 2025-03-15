#!/bin/bash

echo "#################################"
echo "Installing llvm"
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
apt-add-repository 'deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-9 main'
