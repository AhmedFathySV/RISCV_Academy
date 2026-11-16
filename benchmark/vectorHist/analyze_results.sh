#!/bin/bash

OUT=/tmp/hist-runs

echo "=== Histogram Benchmark Analysis ==="
echo ""

get_stat() {
    local stats_file="$1/stats.txt"
    local stat_name="$2"
    if [ -f "$stats_file" ]; then
        # Search for the stat name anywhere in the line, get the last match, and print the 2nd column
        local val=$(grep "$stat_name" "$stats_file" | tail -n 1 | awk '{print $2}')
        if [ -z "$val" ]; then
            echo "NOT_FOUND"
        else
            echo "$val"
        fi
    else
        echo "FILE_MISSING"
    fi
}

declare -a VARIANTS=(
    "scalar-constant" "vector-constant"
    "scalar-uniform" "vector-uniform"
    "scalar-zipf" "vector-zipf"
)

echo "=== Cycle & Instruction Counts ==="
printf "%-20s %15s %15s\n" "Variant" "Ticks" "Insts"
printf "%-20s %15s %15s\n" "-------" "-----" "-----"

for name in "${VARIANTS[@]}"; do
    ticks=$(get_stat "$OUT/$name" "simTicks")
    insts=$(get_stat "$OUT/$name" "simInsts")
    printf "%-20s %15s %15s\n" "$name" "$ticks" "$insts"
done

echo ""
echo "=== Speedup Calculations ==="
echo "(Speedup = Scalar Ticks / Vector Ticks)"
echo ""

calc_speedup() {
    local scalar=$1
    local vector=$2
    if [[ "$vector" =~ ^[0-9]+$ ]] && [ "$vector" -gt 0 ]; then
        awk "BEGIN {printf \"%.2fx\", $scalar / $vector}"
    else
        echo "N/A (Invalid or zero vector ticks)"
    fi
}

echo "Constant speedup:   $(calc_speedup $(get_stat "$OUT/scalar-constant" "simTicks") $(get_stat "$OUT/vector-constant" "simTicks"))"
echo "Uniform speedup:    $(calc_speedup $(get_stat "$OUT/scalar-uniform" "simTicks") $(get_stat "$OUT/vector-uniform" "simTicks"))"
echo "Zipf speedup:       $(calc_speedup $(get_stat "$OUT/scalar-zipf" "simTicks") $(get_stat "$OUT/vector-zipf" "simTicks"))"
