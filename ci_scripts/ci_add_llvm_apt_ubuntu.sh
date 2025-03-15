#!/bin/bash

echo "#################################"
echo "Installing llvm"
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc
apt-add-repository 'deb http://apt.llvm.org/noble/ llvm-toolchain-noble main'
apt-add-repository 'deb http://apt.llvm.org/noble/ llvm-toolchain-noble-20 main'