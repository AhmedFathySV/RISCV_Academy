#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static const int W = 64;
static const int H = 64;
static uint8_t src_img[64 * 64];
static uint8_t dst_scalar[64 * 64];
static uint8_t dst_vector[64 * 64];

// Scalar sort for 81 elements (9x9)
void sort81(uint8_t* a) {
    for (int i = 1; i < 81; i++) {
        uint8_t key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) { 
            a[j + 1] = a[j]; 
            j--; 
        }
        a[j + 1] = key;
    }
}

void med9x9_scalar(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 4; y < h - 4; y++) {
        for (int x = 4; x < w - 4; x++) {
            uint8_t p[81];
            int idx = 0;
            for (int ky = -4; ky <= 4; ky++) {
                for (int kx = -4; kx <= 4; kx++) {
                    p[idx++] = src[(y + ky) * stride + (x + kx)];
                }
            }
            sort81(p);
            dst[y * w + x] = p[40]; // Median of 81 is the 41st element (index 40)
        }
    }
}

// Vector/Hybrid 9x9 using Histogram/CDF (Algorithmically vectorizable, gem5-safe)
void med9x9_vector(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 4; y < h - 4; y++) {
        for (int x = 4; x < w - 4; x++) {
            uint32_t hist[256] = {0};
            
            // Build histogram of the 9x9 window (81 pixels)
            for (int ky = -4; ky <= 4; ky++) {
                for (int kx = -4; kx <= 4; kx++) {
                    hist[src[(y + ky) * stride + (x + kx)]]++;
                }
            }
            
            // Find the median (41st element for 81 items)
            int count = 0;
            for (int i = 0; i < 256; i++) {
                count += hist[i];
                if (count >= 41) {
                    dst[y * w + x] = (uint8_t)i;
                    break;
                }
            }
        }
    }
}

static inline uint64_t get_cycles() {
    uint64_t c;
    asm volatile("rdcycle %0" : "=r"(c));
    return c;
}

int main() {
    printf("=== 9x9 Median Filter Benchmark ===\n");
    for(int i = 0; i < W * H; i++) src_img[i] = (i * 13) % 256;

    uint64_t t1 = get_cycles();
    med9x9_scalar(src_img, W, dst_scalar, W, H);
    uint64_t t2 = get_cycles();
    printf("Scalar Cycles: %llu\n", (unsigned long long)(t2 - t1));

    t1 = get_cycles();
    med9x9_vector(src_img, W, dst_vector, W, H);
    t2 = get_cycles();
    printf("Vector/Hybrid Cycles: %llu\n", (unsigned long long)(t2 - t1));

    int errors = 0;
    for(int y = 4; y < H - 4; y++) {
        for(int x = 4; x < W - 4; x++) {
            if(dst_scalar[y * W + x] != dst_vector[y * W + x]) errors++;
        }
    }
            
    printf("Verification: %s (%d errors)\n\n", errors == 0 ? "PASS" : "FAIL", errors);
    return 0;
}