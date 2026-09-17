#include "image_Add.hpp"
#include <riscv_vector.h>

template <OverFlowPolicy Policy>
static void add_loop(const uint8_t* in0, 
                     const uint8_t* in1, 
                           uint8_t* out,
                     const size_t width, 
                     const size_t height)
{
    size_t vl = 0;

    for (size_t y = 0; y < height; ++y)
    {
        const uint8_t* row0 = &in0[y * width];
        const uint8_t* row1 = &in1[y * width];
        uint8_t* row_out = &out[y * width];

        for (size_t x = 0, length = width; length > 0; length -= vl, x += vl)
        {
            vl = __riscv_vsetvl_e8m8(length);

            const vuint8m8_t a = __riscv_vle8_v_u8m8(&row0[x], vl);
            const vuint8m8_t b = __riscv_vle8_v_u8m8(&row1[x], vl);

            vuint8m8_t sum;
            if constexpr (Policy == OverFlowPolicy::SATURATE)
            {
                sum = __riscv_vsaddu_vv_u8m8(a, b, vl);
            }
            else
            {
                sum = __riscv_vadd_vv_u8m8(a, b, vl);
            }
            __riscv_vse8_v_u8m8(&row_out[x], sum, vl); 
        }
    }
}

void vec::Add(const Image<uint8_t> &input0,
              const Image<uint8_t> &input1,
                    Image<uint8_t> &output,
              const OverFlowPolicy overFlowPolicy)
{
    assert(input0.Width() == input1.Width() && input0.Height() == input1.Height());
    assert(input0.Width() == output.Width() && input0.Height() == output.Height());

    const uint8_t* in0 = input0.GetPtr(0, 0);
    const uint8_t* in1 = input1.GetPtr(0, 0);
    uint8_t* out = output.GetPtr(0, 0);

    const size_t width = input0.Width();
    const size_t height = input0.Height();

    if (overFlowPolicy == OverFlowPolicy::SATURATE)
        add_loop<OverFlowPolicy::SATURATE>(in0, in1, out, width, height);
    else
        add_loop<OverFlowPolicy::WRAP>(in0, in1, out, width, height);
}
