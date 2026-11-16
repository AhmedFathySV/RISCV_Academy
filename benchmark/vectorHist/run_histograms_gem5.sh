#!/bin/bash

GEM5=/home/me/Documents/gem5/GEM5
OUT=/tmp/hist-runs

mkdir -p "$OUT"
cp build/histogram/*.elf "$OUT/"

echo "Running histogram benchmarks on gem5..."

declare -a VARIANTS=(
    "scalar-constant" "vector-constant"
    "scalar-uniform" "vector-uniform"
    "scalar-zipf" "vector-zipf"
)

for name in "${VARIANTS[@]}"; do
    echo ""
    echo "========================================="
    echo "Running: $name"
    echo "========================================="
    
    RUN_DIR="$OUT/$name"
    
    # CRITICAL: Delete old folder to prevent leftover stats.txt files
    rm -rf "$RUN_DIR"
    mkdir -p "$RUN_DIR"
    
    # Run gem5 using O3CPU with the Vector extension explicitly enabled
    "$GEM5/build/RISCV/gem5.opt" \
        --remote-gdb-port=0 \
        -d "$RUN_DIR" \
        "$GEM5/configs/example/se.py" \
        --cpu-type=O3CPU \
        --enable-riscv-vector \
        --cmd="$OUT/${name}.elf" \
        --options=""
    
    echo "Output saved to: $RUN_DIR/stats.txt"
done
echo "All runs complete!"
