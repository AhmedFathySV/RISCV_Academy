#!/bin/bash

# 1. Setup Compiler and Gem5 paths
CXX=riscv64-linux-gnu-g++
CFLAGS="-march=rv64gcv -mabi=lp64d -O2 -static"
GEM5=/home/me/Documents/gem5/GEM5/build/RISCV/gem5.opt
SE_PY=/home/me/Documents/gem5/GEM5/configs/example/se.py

echo "=== Compiling Files ==="

# 2. Compile each .cpp file into an .elf file
# We check if the file exists first so it doesn't crash if you haven't made 7x7 yet.
for size in 3x3 5x5 7x7 9x9 11x11; do
    if [ -f "med${size}.cpp" ]; then
        echo "Compiling med${size}.cpp..."
        $CXX $CFLAGS med${size}.cpp -o med${size}.elf
    fi
done

echo ""
echo "=== Running Benchmarks on gem5 ==="
echo "(Note: -I 0 prevents the 'max instruction count' crash)"

# 3. Run each .elf file on gem5
for size in 3x3 5x5 7x7 9x9 11x11; do
    if [ -f "med${size}.elf" ]; then
        echo ""
        echo "--- Running ${size} ---"
        
        # Clean old results
        rm -rf /tmp/med_${size}
        
        # Run gem5. 
        # IMPORTANT: -I 0 tells gem5 to run until the program finishes, no matter how long it takes!
        $GEM5 -d /tmp/med_${size} $SE_PY \
            --cpu-type=O3CPU \
            --enable-riscv-vector \
            -I 0 \
            --cmd=./med${size}.elf
            
        # Extract and print the cycle count
        echo "Result for ${size}:"
        grep "simTicks" /tmp/med_${size}/stats.txt | tail -n 1
    fi
done

echo ""
echo "=== All Done! ==="
