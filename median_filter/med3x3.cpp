#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <riscv_vector.h>

static const int W = 64;
static const int H = 64;
static uint8_t src_img[64 * 64];
static uint8_t dst_scalar[64 * 64];
static uint8_t dst_vector[64 * 64];

void sort9(uint8_t* a) {
    for (int i = 1; i < 9; i++) {
        uint8_t key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) { a[j + 1] = a[j]; j--; }
        a[j + 1] = key;
    }
}

void med3x3_scalar(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            uint8_t p[9];
            int idx = 0;
            for (int ky = -1; ky <= 1; ky++)
                for (int kx = -1; kx <= 1; kx++)
                    p[idx++] = src[(y + ky) * stride + (x + kx)];
            sort9(p);
            dst[y * w + x] = p[4];
        }
    }
}

void med3x3_vector(const uint8_t* src, int stride, uint8_t* dst, int w, int h) {
    for (int y = 1; y < h - 1; y++) {
        int x = 1;
        while (x < w - 1) {
            size_t vl = __riscv_vsetvl_e8m1(w - 1 - x);
            
            // Load 9 pixels into 9 vector registers
            vuint8m1_t p0 = __riscv_vle8_v_u8m1(src + (y - 1) * stride + x + 0, vl);
            vuint8m1_t p1 = __riscv_vle8_v_u8m1(src + (y - 1) * stride + x + 1, vl);
            vuint8m1_t p2 = __riscv_vle8_v_u8m1(src + (y - 1) * stride + x + 2, vl);
            vuint8m1_t p3 = __riscv_vle8_v_u8m1(src + (y + 0) * stride + x + 0, vl);
            vuint8m1_t p4 = __riscv_vle8_v_u8m1(src + (y + 0) * stride + x + 1, vl);
            vuint8m1_t p5 = __riscv_vle8_v_u8m1(src + (y + 0) * stride + x + 2, vl);
            vuint8m1_t p6 = __riscv_vle8_v_u8m1(src + (y + 1) * stride + x + 0, vl);
            vuint8m1_t p7 = __riscv_vle8_v_u8m1(src + (y + 1) * stride + x + 1, vl);
            vuint8m1_t p8 = __riscv_vle8_v_u8m1(src + (y + 1) * stride + x + 2, vl);

            // Unsigned Compare-Exchange macro (FIXED: uses vminu/vmaxu)
            #define CE_U(a, b) do { \
                vuint8m1_t min_val = __riscv_vminu_vv_u8m1(p##a, p##b, vl); \
                vuint8m1_t max_val = __riscv_vmaxu_vv_u8m1(p##a, p##b, vl); \
                p##a = min_val; p##b = max_val; \
            } while(0)

            // 19-comparator median-of-9 sorting network
            CE_U(1, 2); CE_U(4, 5); CE_U(7, 8); CE_U(0, 1); CE_U(3, 4); CE_U(6, 7); CE_U(1, 2); CE_U(4, 5); CE_U(7, 8);
            CE_U(0, 3); CE_U(5, 8); CE_U(4, 7); CE_U(3, 6); CE_U(1, 4); CE_U(2, 5); CE_U(4, 7); CE_U(4, 2); CE_U(6, 4);
            CE_U(4, 2);
            #undef CE_U

            __riscv_vse8_v_u8m1(dst + y * w + x, p4, vl);
            x += (int)vl;
        }
    }
}

static inline uint64_t get_cycles() {
    uint64_t c;
    asm volatile("rdcycle %0" : "=r"(c));
    return c;
}

int main() {
    printf("=== 3x3 Median Filter Benchmark ===\n");
    for(int i = 0; i < W * H; i++) src_img[i] = (i * 3) % 256;

    uint64_t t1 = get_cycles();
    med3x3_scalar(src_img, W, dst_scalar, W, H);
    uint64_t t2 = get_cycles();
    printf("Scalar Cycles: %llu\n", (unsigned long long)(t2 - t1));

    t1 = get_cycles();
    med3x3_vector(src_img, W, dst_vector, W, H);
    t2 = get_cycles();
    printf("Vector Cycles: %llu\n", (unsigned long long)(t2 - t1));

    int errors = 0;
    for(int y = 1; y < H - 1; y++)
        for(int x = 1; x < W - 1; x++)
            if(dst_scalar[y * W + x] != dst_vector[y * W + x]) errors++;
            
    printf("Verification: %s (%d errors)\n\n", errors == 0 ? "PASS" : "FAIL", errors);
    return 0;
}