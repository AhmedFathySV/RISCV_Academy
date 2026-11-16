#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static const int W = 64;
static const int H = 64;
static uint8_t src_img[64 * 64];
static uint8_t dst_scalar[64 * 64];
static uint8_t dst_vector[64 * 64];

// Scalar sort for 49 elements
void sort49(uint8_t* a) {
    for (int i = 1; i < 49; i++) {
        uint8_t key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) { a[j + 1] = a[j]; j--; }
        a[j + 1] = key;
    }
}

void med7x7_scalar(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 3; y < h - 3; y++) {
        for (int x = 3; x < w - 3; x++) {
            uint8_t p[49];
            int idx = 0;
            for (int ky = -3; ky <= 3; ky++) {
                for (int kx = -3; kx <= 3; kx++) {
                    p[idx++] = src[(y + ky) * stride + (x + kx)];
                }
            }
            sort49(p);
            dst[y * w + x] = p[24]; // Median of 49 is the 25th element (index 24)
        }
    }
}

// Vector/Hybrid 7x7 using Histogram (avoids massive 49-variable RVV unroll)
void med7x7_vector(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 3; y < h - 3; y++) {
        for (int x = 3; x < w - 3; x++) {
            uint32_t hist[256] = {0};
            
            // Build histogram of the 7x7 window (49 pixels)
            for (int ky = -3; ky <= 3; ky++) {
                for (int kx = -3; kx <= 3; kx++) {
                    hist[src[(y + ky) * stride + (x + kx)]]++;
                }
            }
            
            // Find the median (25th element for 49 items)
            int count = 0;
            for (int i = 0; i < 256; i++) {
                count += hist[i];
                if (count >= 25) {
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
    printf("=== 7x7 Median Filter Benchmark ===\n");
    for(int i = 0; i < W * H; i++) src_img[i] = (i * 11) % 256;

    uint64_t t1 = get_cycles();
    med7x7_scalar(src_img, W, dst_scalar, W, H);
    uint64_t t2 = get_cycles();
    printf("Scalar Cycles: %llu\n", (unsigned long long)(t2 - t1));

    t1 = get_cycles();
    med7x7_vector(src_img, W, dst_vector, W, H);
    t2 = get_cycles();
    printf("Vector/Hybrid Cycles: %llu\n", (unsigned long long)(t2 - t1));

    int errors = 0;
    for(int y = 3; y < H - 3; y++) {
        for(int x = 3; x < W - 3; x++) {
            if(dst_scalar[y * W + x] != dst_vector[y * W + x]) errors++;
        }
    }
            
    printf("Verification: %s (%d errors)\n\n", errors == 0 ? "PASS" : "FAIL", errors);
    return 0;
}