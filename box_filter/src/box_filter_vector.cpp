#include "box_filter.hpp"
#include <riscv_vector.h>
#include <vector>
#include <cstring>

// Horizontal pass: vectorized sliding-window sum along each row
// Output is 16-bit to prevent precision loss before vertical pass
static void box_h(const uint8_t* src, uint16_t* tmp, int H, int W,
                  int ksize, int radius, int border_type) {
    const int pad = 2 * radius;
    std::vector<uint8_t> prow(W + pad);

    for (int y = 0; y < H; y++) {
        const uint8_t* s = src + y * W;
        uint8_t fill = (border_type == BORDER_CONSTANT) ? 0 : s[0];
        for (int j = 0; j < radius; j++) prow[j] = fill;
        std::memcpy(&prow[radius], s, W);
        fill = (border_type == BORDER_CONSTANT) ? 0 : s[W - 1];
        for (int j = 0; j < radius; j++) prow[radius + W + j] = fill;

        int x = 0;
        while (x < W) {
            size_t vl = __riscv_vsetvl_e8m1(W - x);
            vuint16m2_t acc = __riscv_vmv_v_x_u16m2(0, vl);

            for (int j = 0; j < ksize; j++) {
                vuint8m1_t v = __riscv_vle8_v_u8m1(&prow[x + j], vl);
                vuint16m2_t vw = __riscv_vwaddu_vx_u16m2(v, 0, vl);
                acc = __riscv_vadd_vv_u16m2(acc, vw, vl);
            }

            // DO NOT DIVIDE HERE. Store 16-bit sum in tmp.
            __riscv_vse16_v_u16m2(&tmp[y * W + x], acc, vl);
            x += vl;
        }
    }
}

// Vertical pass: vectorized sliding-window sum down each column
static void box_v(const uint16_t* tmp, uint8_t* dst, int H, int W,
                  int ksize, int radius, int border_type) {
    int area = ksize * ksize; // Total area for final division
    
    for (int y = 0; y < H; y++) {
        int x = 0;
        while (x < W) {
            size_t vl = __riscv_vsetvl_e16m2(W - x); // 16-bit load
            vuint16m2_t acc = __riscv_vmv_v_x_u16m2(0, vl);

            for (int j = 0; j < ksize; j++) {
                int sy = y + j - radius;
                if (sy < 0 || sy >= H) {
                    if (border_type == BORDER_CONSTANT) {
                        vuint16m2_t zw = __riscv_vmv_v_x_u16m2(0, vl);
                        acc = __riscv_vadd_vv_u16m2(acc, zw, vl);
                        continue;
    }
                    sy = (sy < 0) ? 0 : H - 1; // Replicate
                }
                const uint16_t* row = tmp + sy * W + x;
                vuint16m2_t v = __riscv_vle16_v_u16m2(row, vl);
                acc = __riscv_vadd_vv_u16m2(acc, v, vl);
            }

            // Divide by total area ONCE at the end (matches scalar exactly)
            acc = __riscv_vdivu_vx_u16m2(acc, (uint16_t)area, vl);
            
            // Narrow 16-bit to 8-bit
            vuint8m1_t out = __riscv_vncvt_x_x_w_u8m1(acc, vl);
            __riscv_vse8_v_u8m1(&dst[y * W + x], out, vl);
            x += vl;
        }
    }
}

void box_filter_vector(const uint8_t* src, uint8_t* dst, int H, int W,
                       int ksize, int border_type) {
    if (ksize <= 1) {
        std::memcpy(dst, src, H * W);
        return;
    }
    int radius = ksize / 2;
    // tmp must be 16-bit to hold horizontal sums without precision loss
    std::vector<uint16_t> tmp(H * W);
    box_h(src, tmp.data(), H, W, ksize, radius, border_type);
    box_v(tmp.data(), dst, H, W, ksize, radius, border_type);
}