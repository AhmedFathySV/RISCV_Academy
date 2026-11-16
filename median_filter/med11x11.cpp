#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static const int W = 64;
static const int H = 64;
static uint8_t src_img[64 * 64];
static uint8_t dst_scalar[64 * 64];
static uint8_t dst_vector[64 * 64];

// Scalar sort for 121 elements (11x11)
void sort121(uint8_t* a) {
    for (int i = 1; i < 121; i++) {
        uint8_t key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) { 
            a[j + 1] = a[j]; 
            j--; 
        }
        a[j + 1] = key;
    }
}

void med11x11_scalar(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 5; y < h - 5; y++) {
        for (int x = 5; x < w - 5; x++) {
            uint8_t p[121];
            int idx = 0;
            for (int ky = -5; ky <= 5; ky++) {
                for (int kx = -5; kx <= 5; kx++) {
                    p[idx++] = src[(y + ky) * stride + (x + kx)];
                }
            }
            sort121(p);
            dst[y * w + x] = p[60]; // Median of 121 is the 61st element (index 60)
        }
    }
}

// Vector/Hybrid 11x11 using Histogram/CDF (Algorithmically vectorizable, gem5-safe)
void med11x11_vector(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 5; y < h - 5; y++) {
        for (int x = 5; x < w - 5; x++) {
            uint32_t hist[256] = {0};
            
            // Build histogram of the 11x11 window (121 pixels)
            for (int ky = -5; ky <= 5; ky++) {
                for (int kx = -5; kx <= 5; kx++) {
                    hist[src[(y + ky) * stride + (x + kx)]]++;
                }
            }
            
            // Find the median (61st element for 121 items)
            int count = 0;
            for (int i = 0; i < 256; i++) {
                count += hist[i];
                if (count >= 61) {
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
    printf("=== 11x11 Median Filter Benchmark ===\n");
    for(int i = 0; i < W * H; i++) src_img[i] = (i * 17) % 256;

    uint64_t t1 = get_cycles();
    med11x11_scalar(src_img, W, dst_scalar, W, H);
    uint64_t t2 = get_cycles();
    printf("Scalar Cycles: %llu\n", (unsigned long long)(t2 - t1));

    t1 = get_cycles();
    med11x11_vector(src_img, W, dst_vector, W, H);
    t2 = get_cycles();
    printf("Vector/Hybrid Cycles: %llu\n", (unsigned long long)(t2 - t1));

    int errors = 0;
    for(int y = 5; y < H - 5; y++) {
        for(int x = 5; x < W - 5; x++) {
            if(dst_scalar[y * W + x] != dst_vector[y * W + x]) errors++;
        }
    }
            
    printf("Verification: %s (%d errors)\n\n", errors == 0 ? "PASS" : "FAIL", errors);
    return 0;
}