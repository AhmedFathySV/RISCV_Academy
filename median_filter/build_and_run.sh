#!/bin/bash

CXX=riscv64-linux-gnu-g++
CFLAGS="-march=rv64gcv -mabi=lp64d -O2 -static"
GEM5=/home/me/Documents/gem5/GEM5

echo "=== Building Median Filter Benchmarks ==="

echo "Building med3x3..."
$CXX $CFLAGS med3x3.cpp -o med3x3.elf

echo "Building med5x5..."
$CXX $CFLAGS med5x5.cpp -o med5x5.elf

echo ""
echo "=== Running on gem5 ==="

echo ""
echo "--- Running 3x3 Filter ---"
rm -rf /tmp/med3x3-run
$GEM5/build/RISCV/gem5.opt -d /tmp/med3x3-run \
    $GEM5/configs/example/se.py \
    --cpu-type=O3CPU \
    --enable-riscv-vector \
    --cmd=./med3x3.elf

echo ""
echo "3x3 Results:"
grep "simTicks" /tmp/med3x3-run/stats.txt | tail -n 1

echo ""
echo "--- Running 5x5 Filter ---"
rm -rf /tmp/med5x5-run
$GEM5/build/RISCV/gem5.opt -d /tmp/med5x5-run \
    $GEM5/configs/example/se.py \
    --cpu-type=O3CPU \
    --enable-riscv-vector \
    --cmd=./med5x5.elf

echo ""
echo "5x5 Results:"
grep "simTicks" /tmp/med5x5-run/stats.txt | tail -n 1

echo ""
echo "=== Done! ==="
