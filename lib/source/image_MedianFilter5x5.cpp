#include "image_MedianFilter5x5.hpp"
#include <riscv_vector.h>

namespace vec 
{
    #define SORT2(a, b) do { \
        vuint8m1_t mn = __riscv_vminu_vv_u8m1(a, b, vl); \
        vuint8m1_t mx = __riscv_vmaxu_vv_u8m1(a, b, vl); \
        a = mn; b = mx; \
    } while(0)

    void MedianFilter5x5(const Image<uint8_t>& input, Image<uint8_t>& output)
    {
        int width = input.Width();
        int height = input.Height();

        for (int y = 2; y < height - 2; ++y)
        {
            int x = 2;
            int remaining = width - 4;

            const uint8_t* p_y_minus_2 = input.GetPtr(0, y - 2);
            const uint8_t* p_y_minus_1 = input.GetPtr(0, y - 1);
            const uint8_t* p_y         = input.GetPtr(0, y);
            const uint8_t* p_y_plus_1  = input.GetPtr(0, y + 1);
            const uint8_t* p_y_plus_2  = input.GetPtr(0, y + 2);
            
            uint8_t* out_p = output.GetPtr(0, y);

            while (remaining > 0)
            {
                size_t vl = __riscv_vsetvl_e8m1(remaining);

                vuint8m1_t v0  = __riscv_vle8_v_u8m1(p_y_minus_2 + x - 2, vl);
                vuint8m1_t v1  = __riscv_vle8_v_u8m1(p_y_minus_2 + x - 1, vl);
                vuint8m1_t v2  = __riscv_vle8_v_u8m1(p_y_minus_2 + x    , vl);
                vuint8m1_t v3  = __riscv_vle8_v_u8m1(p_y_minus_2 + x + 1, vl);
                vuint8m1_t v4  = __riscv_vle8_v_u8m1(p_y_minus_2 + x + 2, vl);

                vuint8m1_t v5  = __riscv_vle8_v_u8m1(p_y_minus_1 + x - 2, vl);
                vuint8m1_t v6  = __riscv_vle8_v_u8m1(p_y_minus_1 + x - 1, vl);
                vuint8m1_t v7  = __riscv_vle8_v_u8m1(p_y_minus_1 + x    , vl);
                vuint8m1_t v8  = __riscv_vle8_v_u8m1(p_y_minus_1 + x + 1, vl);
                vuint8m1_t v9  = __riscv_vle8_v_u8m1(p_y_minus_1 + x + 2, vl);

                vuint8m1_t v10 = __riscv_vle8_v_u8m1(p_y + x - 2, vl);
                vuint8m1_t v11 = __riscv_vle8_v_u8m1(p_y + x - 1, vl);
                vuint8m1_t v12 = __riscv_vle8_v_u8m1(p_y + x    , vl);
                vuint8m1_t v13 = __riscv_vle8_v_u8m1(p_y + x + 1, vl);
                vuint8m1_t v14 = __riscv_vle8_v_u8m1(p_y + x + 2, vl);

                vuint8m1_t v15 = __riscv_vle8_v_u8m1(p_y_plus_1 + x - 2, vl);
                vuint8m1_t v16 = __riscv_vle8_v_u8m1(p_y_plus_1 + x - 1, vl);
                vuint8m1_t v17 = __riscv_vle8_v_u8m1(p_y_plus_1 + x    , vl);
                vuint8m1_t v18 = __riscv_vle8_v_u8m1(p_y_plus_1 + x + 1, vl);
                vuint8m1_t v19 = __riscv_vle8_v_u8m1(p_y_plus_1 + x + 2, vl);

                vuint8m1_t v20 = __riscv_vle8_v_u8m1(p_y_plus_2 + x - 2, vl);
                vuint8m1_t v21 = __riscv_vle8_v_u8m1(p_y_plus_2 + x - 1, vl);
                vuint8m1_t v22 = __riscv_vle8_v_u8m1(p_y_plus_2 + x    , vl);
                vuint8m1_t v23 = __riscv_vle8_v_u8m1(p_y_plus_2 + x + 1, vl);
                vuint8m1_t v24 = __riscv_vle8_v_u8m1(p_y_plus_2 + x + 2, vl);

                SORT2(v0,v1);  SORT2(v0,v2);  SORT2(v0,v3);  SORT2(v0,v4);  SORT2(v0,v5);  SORT2(v0,v6);  SORT2(v0,v7);  SORT2(v0,v8);  SORT2(v0,v9);  SORT2(v0,v10); SORT2(v0,v11); SORT2(v0,v12); SORT2(v0,v13); SORT2(v0,v14); SORT2(v0,v15); SORT2(v0,v16); SORT2(v0,v17); SORT2(v0,v18); SORT2(v0,v19); SORT2(v0,v20); SORT2(v0,v21); SORT2(v0,v22); SORT2(v0,v23); SORT2(v0,v24);
                SORT2(v1,v2);  SORT2(v1,v3);  SORT2(v1,v4);  SORT2(v1,v5);  SORT2(v1,v6);  SORT2(v1,v7);  SORT2(v1,v8);  SORT2(v1,v9);  SORT2(v1,v10); SORT2(v1,v11); SORT2(v1,v12); SORT2(v1,v13); SORT2(v1,v14); SORT2(v1,v15); SORT2(v1,v16); SORT2(v1,v17); SORT2(v1,v18); SORT2(v1,v19); SORT2(v1,v20); SORT2(v1,v21); SORT2(v1,v22); SORT2(v1,v23); SORT2(v1,v24);
                SORT2(v2,v3);  SORT2(v2,v4);  SORT2(v2,v5);  SORT2(v2,v6);  SORT2(v2,v7);  SORT2(v2,v8);  SORT2(v2,v9);  SORT2(v2,v10); SORT2(v2,v11); SORT2(v2,v12); SORT2(v2,v13); SORT2(v2,v14); SORT2(v2,v15); SORT2(v2,v16); SORT2(v2,v17); SORT2(v2,v18); SORT2(v2,v19); SORT2(v2,v20); SORT2(v2,v21); SORT2(v2,v22); SORT2(v2,v23); SORT2(v2,v24);
                SORT2(v3,v4);  SORT2(v3,v5);  SORT2(v3,v6);  SORT2(v3,v7);  SORT2(v3,v8);  SORT2(v3,v9);  SORT2(v3,v10); SORT2(v3,v11); SORT2(v3,v12); SORT2(v3,v13); SORT2(v3,v14); SORT2(v3,v15); SORT2(v3,v16); SORT2(v3,v17); SORT2(v3,v18); SORT2(v3,v19); SORT2(v3,v20); SORT2(v3,v21); SORT2(v3,v22); SORT2(v3,v23); SORT2(v3,v24);
                SORT2(v4,v5);  SORT2(v4,v6);  SORT2(v4,v7);  SORT2(v4,v8);  SORT2(v4,v9);  SORT2(v4,v10); SORT2(v4,v11); SORT2(v4,v12); SORT2(v4,v13); SORT2(v4,v14); SORT2(v4,v15); SORT2(v4,v16); SORT2(v4,v17); SORT2(v4,v18); SORT2(v4,v19); SORT2(v4,v20); SORT2(v4,v21); SORT2(v4,v22); SORT2(v4,v23); SORT2(v4,v24);
                SORT2(v5,v6);  SORT2(v5,v7);  SORT2(v5,v8);  SORT2(v5,v9);  SORT2(v5,v10); SORT2(v5,v11); SORT2(v5,v12); SORT2(v5,v13); SORT2(v5,v14); SORT2(v5,v15); SORT2(v5,v16); SORT2(v5,v17); SORT2(v5,v18); SORT2(v5,v19); SORT2(v5,v20); SORT2(v5,v21); SORT2(v5,v22); SORT2(v5,v23); SORT2(v5,v24);
                SORT2(v6,v7);  SORT2(v6,v8);  SORT2(v6,v9);  SORT2(v6,v10); SORT2(v6,v11); SORT2(v6,v12); SORT2(v6,v13); SORT2(v6,v14); SORT2(v6,v15); SORT2(v6,v16); SORT2(v6,v17); SORT2(v6,v18); SORT2(v6,v19); SORT2(v6,v20); SORT2(v6,v21); SORT2(v6,v22); SORT2(v6,v23); SORT2(v6,v24);
                SORT2(v7,v8);  SORT2(v7,v9);  SORT2(v7,v10); SORT2(v7,v11); SORT2(v7,v12); SORT2(v7,v13); SORT2(v7,v14); SORT2(v7,v15); SORT2(v7,v16); SORT2(v7,v17); SORT2(v7,v18); SORT2(v7,v19); SORT2(v7,v20); SORT2(v7,v21); SORT2(v7,v22); SORT2(v7,v23); SORT2(v7,v24);
                SORT2(v8,v9);  SORT2(v8,v10); SORT2(v8,v11); SORT2(v8,v12); SORT2(v8,v13); SORT2(v8,v14); SORT2(v8,v15); SORT2(v8,v16); SORT2(v8,v17); SORT2(v8,v18); SORT2(v8,v19); SORT2(v8,v20); SORT2(v8,v21); SORT2(v8,v22); SORT2(v8,v23); SORT2(v8,v24);
                SORT2(v9,v10); SORT2(v9,v11); SORT2(v9,v12); SORT2(v9,v13); SORT2(v9,v14); SORT2(v9,v15); SORT2(v9,v16); SORT2(v9,v17); SORT2(v9,v18); SORT2(v9,v19); SORT2(v9,v20); SORT2(v9,v21); SORT2(v9,v22); SORT2(v9,v23); SORT2(v9,v24);
                SORT2(v10,v11); SORT2(v10,v12); SORT2(v10,v13); SORT2(v10,v14); SORT2(v10,v15); SORT2(v10,v16); SORT2(v10,v17); SORT2(v10,v18); SORT2(v10,v19); SORT2(v10,v20); SORT2(v10,v21); SORT2(v10,v22); SORT2(v10,v23); SORT2(v10,v24);
                SORT2(v11,v12); SORT2(v11,v13); SORT2(v11,v14); SORT2(v11,v15); SORT2(v11,v16); SORT2(v11,v17); SORT2(v11,v18); SORT2(v11,v19); SORT2(v11,v20); SORT2(v11,v21); SORT2(v11,v22); SORT2(v11,v23); SORT2(v11,v24);
                SORT2(v12,v13); SORT2(v12,v14); SORT2(v12,v15); SORT2(v12,v16); SORT2(v12,v17); SORT2(v12,v18); SORT2(v12,v19); SORT2(v12,v20); SORT2(v12,v21); SORT2(v12,v22); SORT2(v12,v23); SORT2(v12,v24);

                __riscv_vse8_v_u8m1(out_p + x, v12, vl);

                x += vl;
                remaining -= vl;
            }
        }
    }
}