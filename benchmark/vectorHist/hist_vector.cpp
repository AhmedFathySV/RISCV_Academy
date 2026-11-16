#include <stdint.h>
#include <stddef.h>
#include <riscv_vector.h>

// Allocate enough space for the maximum possible VLMAX (256 lanes)
// 256 bins * 256 max lanes = 65,536 elements = 256 KB.
static uint32_t priv_array[256 * 256]; 

void compute_histogram_vector(const uint8_t* image, size_t n, uint32_t* hist) {
    // Clear final histogram
    for(int i = 0; i < 256; i++) {
        hist[i] = 0;
    }

    // Get the ACTUAL maximum vector length for 8-bit elements
    size_t true_vlmax = __riscv_vsetvlmax_e8m1();
    
    // Safety cap: ensure we don't exceed our statically allocated array bounds
    if (true_vlmax > 256) true_vlmax = 256;

    // Clear the private array up to the safe limit
    for (size_t i = 0; i < 256 * true_vlmax; i++) {
        priv_array[i] = 0;
    }

    size_t i = 0;
    while (i < n) {
        // Set vector length for this iteration. 
        size_t vl = __riscv_vsetvl_e8m1(n - i);
        
        // 2. Load pixels (8-bit)
        vuint8m1_t v_pixels = __riscv_vle8_v_u8m1(image + i, vl);
        
        // 3. Get Lane IDs (0, 1, 2, ..., vl-1)
        vuint32m4_t v_lane = __riscv_vid_v_u32m4(vl);
        
        // 4. Calculate Slots: slot = (bin * true_vlmax) + lane
        vuint32m4_t v_bin = __riscv_vzext_vf4_u32m4(v_pixels, vl);
        
        vuint32m4_t v_slot = __riscv_vadd_vv_u32m4(
            __riscv_vmul_vx_u32m4(v_bin, true_vlmax, vl), 
            v_lane, 
            vl
        );
        
        // 5. Calculate byte offset for uint32_t array: off = slot * 4
        vuint32m4_t v_off = __riscv_vsll_vx_u32m4(v_slot, 2, vl);
        
        // 6. Gather current counts from priv array
        vuint32m4_t v_counts = __riscv_vluxei32_v_u32m4(priv_array, v_off, vl);
        
        // 7. Increment counts by 1
        vuint32m4_t v_new_counts = __riscv_vadd_vx_u32m4(v_counts, 1, vl);
        
        // 8. Scatter new counts back to priv array
        __riscv_vsuxei32_v_u32m4(priv_array, v_off, v_new_counts, vl);
        
        i += vl;
    }

    // 9. Reduction (Level 2): Sum across lanes to get final hist
    for (int bin = 0; bin < 256; ++bin) {
        uint32_t sum = 0;
        for (size_t lane = 0; lane < true_vlmax; ++lane) {
            sum += priv_array[bin * true_vlmax + lane];
        }
        hist[bin] = sum;
    }
}

// NAÏVE VECTOR IMPLEMENTATION (Loses counts due to race conditions)
void compute_histogram_naive_vector(const uint8_t* image, size_t n, uint32_t* hist) {
    // Clear histogram
    for(int i = 0; i < 256; i++) {
        hist[i] = 0;
    }

    size_t i = 0;
    while (i < n) {
        size_t vl = __riscv_vsetvl_e8m1(n - i);
        
        // Load pixels
        vuint8m1_t v_pixels = __riscv_vle8_v_u8m1(image + i, vl);
        
        // Extend to 32-bit for addressing
        vuint32m4_t v_bin = __riscv_vzext_vf4_u32m4(v_pixels, vl);
        
        // Calculate byte offset: bin * 4
        vuint32m4_t v_off = __riscv_vsll_vx_u32m4(v_bin, 2, vl);
        
        // GATHER: Load current counts (Naïve: reading from global hist)
        vuint32m4_t v_counts = __riscv_vluxei32_v_u32m4(hist, v_off, vl);
        
        // Add 1
        vuint32m4_t v_new_counts = __riscv_vadd_vx_u32m4(v_counts, 1, vl);
        
        // SCATTER: Store back (Naïve: writing to global hist)
        // BUG: If two lanes have the same pixel value, they overwrite each other!
        __riscv_vsuxei32_v_u32m4(hist, v_off, v_new_counts, vl);
        
        i += vl;
    }
}