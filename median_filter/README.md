# Vectorised Median Filters (3x3 … 11x11) – gem5 / RISC-V RVV

This project implements and benchmarks scalar and vectorized (RISC-V Vector / RVV) median filters for kernel sizes ranging from 3x3 to 11x11. It demonstrates the algorithmic crossover point where vectorized/hybrid approaches begin to significantly outperform scalar baselines.

## 📁 File Structure & Algorithms

To ensure maximum compatibility with the gem5 `se.py` simulator and avoid known startup initialization bugs, each kernel size is implemented as a standalone, self-contained `.cpp` file.

| File | Scalar Algorithm | Vector / Hybrid Algorithm |
|---|---|---|
| `med3x3.cpp` | Insertion sort (9 elements) | **19-Comparator Median-of-9 Sorting Network** (using RVV `vminu`/`vmaxu`) |
| `med5x5.cpp` | Insertion sort (25 elements) | **Histogram / CDF Approach** (avoids $O(N^2)$ scalar blowup) |
| `med7x7.cpp` | Insertion sort (49 elements) | **Histogram / CDF Approach** |
| `med9x9.cpp` | Insertion sort (81 elements) | **Histogram / CDF Approach** |
| `med11x11.cpp`| Insertion sort (121 elements)| **Histogram / CDF Approach** |

*(Note: Earlier attempts using complex `.h` wrapper templates were replaced with standalone files to bypass gem5 `se.py` `mstatus.VS` initialization panics during program startup).*

---

## 📊 Final Benchmark Results (64x64 Image)

The results clearly demonstrate the **crossover point**: for small kernels (5x5), the fixed overhead of the histogram approach makes it slightly slower than simple scalar sorting. However, as kernel size grows, the $O(N^2)$ complexity of scalar sorting causes execution time to explode, while the histogram approach remains stable, yielding massive speedups.

| Kernel Size | Scalar Cycles | Vector/Hybrid Cycles | Speedup (Scalar / Vector) | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **3x3** | 960,241 | N/A | N/A | Algorithm verified on host. gem5 `se.py` fails to initialize vector state (`VS off` panic) for this specific RVV sorting network binary. |
| **5x5** | 897,664 | 1,137,410 | **0.79x** (Slower) | Histogram overhead outweighs benefits for small kernels. |
| **7x7** | 2,727,951 | 1,152,423 | **2.37x** (Faster) | Scalar sorting cost begins to exceed histogram overhead. |
| **9x9** | 5,966,907 | 1,268,295 | **4.70x** (Much Faster) | Histogram approach dominates. |
| **11x11**| 13,192,995 | 1,391,914 | **9.48x** (Massive Speedup)| Scalar $O(N^2)$ bottleneck is fully exposed. |

---

## 🛠️ Workflow: How to Compile and Run

We use a direct compilation and execution workflow to ensure reliability. 

### 1. Compile a Specific Kernel
Use the RISC-V cross-compiler with the Vector extension enabled (`rv64gcv`).
```bash
# Example: Compile 9x9
riscv64-linux-gnu-g++ -march=rv64gcv -mabi=lp64d -O2 -static med9x9.cpp -o med9x9.elf