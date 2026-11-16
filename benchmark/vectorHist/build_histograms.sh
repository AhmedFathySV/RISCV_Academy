#!/bin/bash

# Use the Linux cross-compiler (has full stdio.h and stdint.h support)
CXX=riscv64-linux-gnu-g++

# Flags: Enable Vector extension, optimize, and link statically for gem5
CFLAGS="-march=rv64gcv -mabi=lp64d -O2 -static"

# Output directory
mkdir -p build/histogram

echo "Building 6 histogram variants..."

# Define variants
declare -a METHODS=("scalar" "vector")
declare -a DATASETS=("constant" "uniform" "zipf")
declare -a METHOD_FLAGS=("0" "1")
declare -a DATASET_FLAGS=("0" "1" "2")

# Build all combinations
for i in 0 1; do
    for j in 0 1 2; do
        METHOD_NAME=${METHODS[$i]}
        DATASET_NAME=${DATASETS[$j]}
        METHOD_FLAG=${METHOD_FLAGS[$i]}
        DATASET_FLAG=${DATASET_FLAGS[$j]}
        
        OUTPUT_NAME="${METHOD_NAME}-${DATASET_NAME}"
        OUTPUT_ELF="build/histogram/${OUTPUT_NAME}.elf"
        OUTPUT_BIN="build/histogram/${OUTPUT_NAME}.bin"
        
        echo "Building ${OUTPUT_NAME}..."
        
        $CXX $CFLAGS \
            -DHIST_METHOD=${METHOD_FLAG} \
            -DHIST_DATASET=${DATASET_FLAG} \
            main.cpp hist_scalar.cpp hist_vector.cpp \
            -o ${OUTPUT_ELF}
        
        # Convert to binary for gem5
        riscv64-linux-gnu-objcopy -O binary ${OUTPUT_ELF} ${OUTPUT_BIN}
        
        echo "  Created: ${OUTPUT_ELF}"
        echo "  Created: ${OUTPUT_BIN}"
    done
done

echo "Build complete! All variants in build/histogram/"