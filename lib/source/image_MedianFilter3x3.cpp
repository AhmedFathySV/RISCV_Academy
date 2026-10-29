#include "image_MedianFilter3x3.hpp"
#include <riscv_vector.h>

namespace vec 
{
    inline void sort_network_step(vuint8m1_t &a, vuint8m1_t &b, size_t vl) {
        vuint8m1_t min_val = __riscv_vminu_vv_u8m1(a, b, vl);
        vuint8m1_t max_val = __riscv_vmaxu_vv_u8m1(a, b, vl);
        a = min_val;
        b = max_val;
    }

    void MedianFilter3x3(const Image<uint8_t>& input, Image<uint8_t>& output)
    {
        int width = input.Width();
        int height = input.Height();
        
        for (int y = 1; y < height - 1; ++y)
        {
            int x = 1;
            int remaining = width - 2;
            
            while (remaining > 0)
            {
                size_t vl = __riscv_vsetvl_e8m1(remaining);
                
                const uint8_t* p0 = input.GetPtr(x, y - 1);
                const uint8_t* p1 = input.GetPtr(x, y);
                const uint8_t* p2 = input.GetPtr(x, y + 1);

                vuint8m1_t v0 = __riscv_vle8_v_u8m1(p0 - 1, vl);
                vuint8m1_t v1 = __riscv_vle8_v_u8m1(p0,     vl);
                vuint8m1_t v2 = __riscv_vle8_v_u8m1(p0 + 1, vl);
                vuint8m1_t v3 = __riscv_vle8_v_u8m1(p1 - 1, vl);
                vuint8m1_t v4 = __riscv_vle8_v_u8m1(p1,     vl);
                vuint8m1_t v5 = __riscv_vle8_v_u8m1(p1 + 1, vl);
                vuint8m1_t v6 = __riscv_vle8_v_u8m1(p2 - 1, vl);
                vuint8m1_t v7 = __riscv_vle8_v_u8m1(p2,     vl);
                vuint8m1_t v8 = __riscv_vle8_v_u8m1(p2 + 1, vl);

                sort_network_step(v1, v2, vl); sort_network_step(v4, v5, vl); sort_network_step(v7, v8, vl);
                sort_network_step(v0, v1, vl); sort_network_step(v3, v4, vl); sort_network_step(v6, v7, vl);
                sort_network_step(v1, v2, vl); sort_network_step(v4, v5, vl); sort_network_step(v7, v8, vl);
                sort_network_step(v0, v3, vl); sort_network_step(v5, v8, vl); sort_network_step(v4, v7, vl);
                sort_network_step(v3, v6, vl); sort_network_step(v1, v4, vl); sort_network_step(v2, v5, vl);
                sort_network_step(v4, v7, vl); sort_network_step(v4, v2, vl); sort_network_step(v6, v4, vl);
                sort_network_step(v4, v2, vl);
                
                __riscv_vse8_v_u8m1(output.GetPtr(x, y), v4, vl);
                
                x += vl;
                remaining -= vl;
            }
        }
    }
}