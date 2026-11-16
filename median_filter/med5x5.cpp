#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static const int W = 64;
static const int H = 64;
static uint8_t src_img[64 * 64];
static uint8_t dst_scalar[64 * 64];
static uint8_t dst_vector[64 * 64];

void sort25(uint8_t* a) {
    for (int i = 1; i < 25; i++) {
        uint8_t key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) { a[j + 1] = a[j]; j--; }
        a[j + 1] = key;
    }
}

void med5x5_scalar(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 2; y < h - 2; y++) {
        for (int x = 2; x < w - 2; x++) {
            uint8_t p[25];
            int idx = 0;
            for (int ky = -2; ky <= 2; ky++)
                for (int kx = -2; kx <= 2; kx++)
                    p[idx++] = src[(y + ky) * stride + (x + kx)];
            sort25(p);
            dst[y * w + x] = p[12];
        }
    }
}

// Vector/Hybrid 5x5 using Histogram (avoids massive 25-variable RVV unroll)
void med5x5_vector(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 2; y < h - 2; y++) {
        for (int x = 2; x < w - 2; x++) {
            uint32_t hist[256] = {0};
            
            // Build histogram of the 5x5 window
            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    hist[src[(y + ky) * stride + (x + kx)]]++;
                }
            }
            
            // Find the median (13th element for 25 items)
            int count = 0;
            for (int i = 0; i < 256; i++) {
                count += hist[i];
                if (count >= 13) {
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
    printf("=== 5x5 Median Filter Benchmark ===\n");
    for(int i = 0; i < W * H; i++) src_img[i] = (i * 7) % 256;

    uint64_t t1 = get_cycles();
    med5x5_scalar(src_img, W, dst_scalar, W, H);
    uint64_t t2 = get_cycles();
    printf("Scalar Cycles: %llu\n", (unsigned long long)(t2 - t1));

    t1 = get_cycles();
    med5x5_vector(src_img, W, dst_vector, W, H);
    t2 = get_cycles();
    printf("Vector/Hybrid Cycles: %llu\n", (unsigned long long)(t2 - t1));

    int errors = 0;
    for(int y = 2; y < H - 2; y++)
        for(int x = 2; x < W - 2; x++)
            if(dst_scalar[y * W + x] != dst_vector[y * W + x]) errors++;
            
    printf("Verification: %s (%d errors)\n\n", errors == 0 ? "PASS" : "FAIL", errors);
    return 0;
}