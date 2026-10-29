#include "image_MedianFilter9x9.hpp"
#include <riscv_vector.h>

namespace vec 
{
    void MedianFilter9x9(const Image<uint8_t>& input, Image<uint8_t>& output)
    {
        int width = input.Width();
        int height = input.Height();

        constexpr int MAX_VL = 64; 
        uint8_t hist[MAX_VL][256]; 
        uint8_t temp_old[MAX_VL];
        uint8_t temp_new[MAX_VL];
        uint8_t medians[MAX_VL];

        for (int x = 4; x < width - 4; )
        {
            int remaining = width - 4 - x;
            size_t vl = __riscv_vsetvl_e8m1(remaining); 

            __builtin_memset(hist, 0, sizeof(hist));
            for (int dy = -4; dy <= 4; ++dy)
            {
                for (int dx = -4; dx <= 4; ++dx)
                {
                    const uint8_t* ptr = input.GetPtr(x + dx, 4 + dy);
                    vuint8m1_t v_pixels = __riscv_vle8_v_u8m1(ptr, vl);
                    
                    __riscv_vse8_v_u8m1(temp_new, v_pixels, vl);
                    
                    for (size_t l = 0; l < vl; ++l)
                    {
                        hist[l][temp_new[l]]++; 
                    }
                }
            }

            for (size_t l = 0; l < vl; ++l)
            {
                int count = 0;
                for (int bin = 0; bin < 256; ++bin)
                {
                    count += hist[l][bin];
                    if (count >= 41) 
                    {
                        medians[l] = bin;
                        break;
                    }
                }
            }
            vuint8m1_t v_med = __riscv_vle8_v_u8m1(medians, vl);
            __riscv_vse8_v_u8m1(output.GetPtr(x, 4), v_med, vl);

            for (int y = 5; y < height - 4; ++y)
            {
                int old_y = y - 5; 
                int new_y = y + 4; 

                for (int dx = -4; dx <= 4; ++dx)
                {
                    const uint8_t* ptr_old = input.GetPtr(x + dx, old_y);
                    vuint8m1_t v_old = __riscv_vle8_v_u8m1(ptr_old, vl);
                    __riscv_vse8_v_u8m1(temp_old, v_old, vl);

                    const uint8_t* ptr_new = input.GetPtr(x + dx, new_y);
                    vuint8m1_t v_new = __riscv_vle8_v_u8m1(ptr_new, vl);
                    __riscv_vse8_v_u8m1(temp_new, v_new, vl);

                    for (size_t l = 0; l < vl; ++l)
                    {
                        hist[l][temp_old[l]]--; 
                        hist[l][temp_new[l]]++; 
                    }
                }

                for (size_t l = 0; l < vl; ++l)
                {
                    int count = 0;
                    for (int bin = 0; bin < 256; ++bin)
                    {
                        count += hist[l][bin];
                        if (count >= 41)
                        {
                            medians[l] = bin;
                            break;
                        }
                    }
                }

                v_med = __riscv_vle8_v_u8m1(medians, vl);
                __riscv_vse8_v_u8m1(output.GetPtr(x, y), v_med, vl);
            }

            x += vl; 
        }
    }
}