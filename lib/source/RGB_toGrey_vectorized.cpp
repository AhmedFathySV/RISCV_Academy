#include <riscv_vector.h>
#include "RGB_toGrey_vectorized.hpp"
#include "image.hpp" 

void vec::RGB_toGrey_vectorized(const Image<uint8_t, ImageType::RGB>& RGBImage, 
                           Image<uint8_t, ImageType::GRAY>& output) 
{
    const int width = RGBImage.Width();
    const int height = RGBImage.Height();

    // Guard against empty images
    if (width <= 0 || height <= 0) {
        return;
    }

    // Allocate/reallocate output image if dimensions do not match
    if (output.Width() != width || output.Height() != height) {
        output = Image<uint8_t, ImageType::GRAY>(width, height);
    }

    // Base raw pointers to data buffers
    const uint8_t* src = RGBImage.GetPtr(0, 0);
    uint8_t* dst = output.GetPtr(0, 0);

    const size_t total_pixels = static_cast<size_t>(width) * height;
    size_t i = 0;

    // Q0.16 fixed-point weights
    constexpr uint32_t R_WEIGHT = 19595; // 0.299 * 65536
    constexpr uint32_t G_WEIGHT = 38470; // 0.587 * 65536
    constexpr uint32_t B_WEIGHT = 7471;  // 0.114 * 65536
    constexpr uint32_t ROUNDING = 32768; // 0.5   * 65536
    const ptrdiff_t stride = 3; // 3-byte pixel stride

    while (i < total_pixels) {
        // 1. Query hardware vector length for remaining elements (LMUL = 1)
        size_t vl = __riscv_vsetvl_e8m1(total_pixels - i);

        // 2. Segmented load: de-interleave RGB bytes into 3 separate 8-bit vector registers
        // auto rgb     = __riscv_vlseg3e8_v_u8m1(src + (i * 3), vl);
        // vuint8m1_t r = __riscv_vget_v_u8m1(rgb, 0);
        // vuint8m1_t g = __riscv_vget_v_u8m1(rgb, 1);
        // vuint8m1_t b = __riscv_vget_v_u8m1(rgb, 2);


        vuint8m1_t r = __riscv_vlse8_v_u8m1(src + (i * 3) + 0, stride, vl);
        vuint8m1_t g = __riscv_vlse8_v_u8m1(src + (i * 3) + 1, stride, vl);
        vuint8m1_t b = __riscv_vlse8_v_u8m1(src + (i * 3) + 2, stride, vl);


        // 3. Zero-extension (u8 -> u16 -> u32) to prevent accumulation overflow
        vuint16m2_t r16 = __riscv_vwcvtu_x_x_v_u16m2(r, vl);
        vuint16m2_t g16 = __riscv_vwcvtu_x_x_v_u16m2(g, vl);
        vuint16m2_t b16 = __riscv_vwcvtu_x_x_v_u16m2(b, vl);

        vuint32m4_t r32 = __riscv_vwcvtu_x_x_v_u32m4(r16, vl);
        vuint32m4_t g32 = __riscv_vwcvtu_x_x_v_u32m4(g16, vl);
        vuint32m4_t b32 = __riscv_vwcvtu_x_x_v_u32m4(b16, vl);

        // 4. Initialize accumulator register with rounding offset
        vuint32m4_t acc = __riscv_vmv_v_x_u32m4(ROUNDING, vl);

        // 5. Multiply-accumulate channel weights into 32-bit vector
        acc = __riscv_vmacc_vx_u32m4(acc, R_WEIGHT, r32, vl);
        acc = __riscv_vmacc_vx_u32m4(acc, G_WEIGHT, g32, vl);
        acc = __riscv_vmacc_vx_u32m4(acc, B_WEIGHT, b32, vl);

        // 6. Divide by 65536 via right shift (16 bits)
        vuint32m4_t shifted = __riscv_vsrl_vx_u32m4(acc, 16, vl);

        // 7. Narrowing truncation (u32 -> u16 -> u8)
        vuint16m2_t grey16 = __riscv_vncvt_x_x_w_u16m2(shifted, vl);
        vuint8m1_t grey8   = __riscv_vncvt_x_x_w_u8m1(grey16, vl);

        // 8. Vector store grayscale output
        __riscv_vse8_v_u8m1(dst + i, grey8, vl);

        i += vl;
    }
}