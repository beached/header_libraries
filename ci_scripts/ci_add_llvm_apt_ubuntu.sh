#!/bin/bash

echo "#################################"
echo "Installing llvm"
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
apt-add-repository 'deb http://apt.llvm.org/noble/ llvm-toolchain-noble main'
apt-add-repository 'deb http://apt.llvm.org/noble/ llvm-toolchain-noble-20 main'