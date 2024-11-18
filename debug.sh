#!/bin/bash

set -e

cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build

./build/compiler -koopa ./test/lv1/0_main.c -o ./result/ir.out
./build/compiler -riscv ./test/lv1/0_main.c -o ./result/asm.out
