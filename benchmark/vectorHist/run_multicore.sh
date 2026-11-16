#!/bin/bash
GEM5=/home/me/Documents/gem5/GEM5
OUT=/tmp/hist-multicore

mkdir -p "$OUT"
cp build/histogram/scalar-zipf.elf "$OUT/"

echo "Running 2 Cores (TimingSimpleCPU)..."

# --work-end-exit-count=2 tells gem5 to wait for BOTH cores to finish before exiting.
# This prevents the "Commit stage stuck" panic.
"$GEM5/build/RISCV/gem5.opt" \
    -d "$OUT/dual-core-zipf" \
    "$GEM5/configs/example/se.py" \
    --cpu-type=TimingSimpleCPU \
    --l2cache \
    -n 2 \
    --work-end-exit-count=2 \
    --cmd="$OUT/scalar-zipf.elf" \
    --cmd="$OUT/scalar-zipf.elf"

echo "Done."
