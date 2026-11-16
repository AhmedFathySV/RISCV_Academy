#!/bin/bash
GEM5=/home/me/Documents/gem5/GEM5
OUT=/tmp/hist-prefetch-on

mkdir -p "$OUT"
cp build/histogram/*.elf "$OUT/"

echo "Running Scalar-Zipf WITH Stride Prefetcher..."
"$GEM5/build/RISCV/gem5.opt" \
    -d "$OUT/scalar-zipf-with-pf" \
    "$GEM5/configs/example/se.py" \
    --cpu-type=O3CPU \
    --enable-riscv-vector \
    --l1d-hwp-type=StridePrefetcher \
    --l2-hwp-type=TaggedPrefetcher \
    --cmd="$OUT/scalar-zipf.elf"

echo "Done."
