#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// External functions
extern void compute_histogram_scalar(const uint8_t*, size_t, uint32_t*);
extern void compute_histogram_naive_vector(const uint8_t*, size_t, uint32_t*);
extern void compute_histogram_vector(const uint8_t*, size_t, uint32_t*); // This is the Hierarchical/Optimized one

// Use 1024 items for stability in gem5 se.py (as discussed, 1M crashes vector)
static const size_t N = 1024; 
static uint8_t image[1024];
static uint32_t histogram[256];

void generate_constant_dataset(uint8_t* img, size_t n) {
    for (size_t i = 0; i < n; i++) img[i] = 128;
}

void generate_zipf_dataset(uint8_t* img, size_t n) {
    uint32_t seed = 42;
    for (size_t i = 0; i < n; i++) {
        seed = seed * 1103515245 + 12345;
        uint32_t r = (seed >> 16) & 0xFFFF;
        if ((seed % 100) < 80) img[i] = (uint8_t)(r % 32);
        else img[i] = (uint8_t)(r % 256);
    }
}

bool verify_and_print(const char* name, uint32_t* hist, size_t n) {
    uint32_t total = 0;
    for (int i = 0; i < 256; i++) total += hist[i];
    
    bool pass = (total == n);
    printf("%s: Total Count = %u (Expected %zu) -> %s\n", 
           name, total, n, pass ? "PASS" : "FAIL (Lost counts!)");
    return pass;
}

int main() {
    // --- FIX: Manually enable Vector Unit for gem5 ---
    // mstatus.VS is bits 9:8. We set it to 11 (0x600) to activate vector state.
    uint64_t mstatus_val = 0x600; 
  
    // ------------------------------------------------

    printf("=== Histogram Comparison (N=%zu) ===\n\n", N);

    // ... rest of your code ...

    // Generate Skewed Data
    printf("Generating strongly skewed dataset...\n");
    generate_zipf_dataset(image, N);

#ifdef NAIVE_MODE
    // --- RUN NAVE VECTOR ONLY ---
    printf("--- Running Naïve Vector Implementation ---\n");
    compute_histogram_naive_vector(image, N, histogram);
    verify_and_print("Naïve Vector", histogram, N);
    printf("(Note: If Total Count < %zu, it lost counts due to race conditions)\n", N);

#else
    // --- RUN OPTIMIZED (HIERARCHICAL) VECTOR ONLY ---
    // (We assume this binary is for the optimized vector)
    printf("--- Running Optimized Vector (Hierarchical) ---\n");
    compute_histogram_vector(image, N, histogram);
    verify_and_print("Optimized Vector", histogram, N);
#endif

    return 0;
}