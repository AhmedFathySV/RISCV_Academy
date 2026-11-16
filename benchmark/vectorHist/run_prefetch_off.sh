#!/bin/bash
GEM5=/home/me/Documents/gem5/GEM5
OUT=/tmp/hist-prefetch-off

mkdir -p "$OUT"
cp build/histogram/*.elf "$OUT/"

echo "Running Scalar-Zipf with NO Prefetcher..."
"$GEM5/build/RISCV/gem5.opt" \
    -d "$OUT/scalar-zipf-no-pf" \
    "$GEM5/configs/example/se.py" \
    --cpu-type=O3CPU \
    --enable-riscv-vector \
    --no-pf \
    --cmd="$OUT/scalar-zipf.elf"

echo "Done."
