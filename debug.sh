#!/bin/bash

set -e

cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build

./build/compiler -koopa ./test/temp.c -o ./result/ir.out
./build/compiler -riscv ./test/temp.c -o ./result/asm.out
