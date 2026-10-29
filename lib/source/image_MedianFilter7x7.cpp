#include "image_MedianFilter7x7.hpp"
#include <riscv_vector.h>

namespace vec 
{
    #define SORT2(a, b) do { \
        vuint8m1_t mn = __riscv_vminu_vv_u8m1(a, b, vl); \
        vuint8m1_t mx = __riscv_vmaxu_vv_u8m1(a, b, vl); \
        a = mn; b = mx; \
    } while(0)

    #define BUBBLE_SORT_7(v0, v1, v2, v3, v4, v5, v6) do { \
        SORT2(v0,v1); SORT2(v1,v2); SORT2(v2,v3); SORT2(v3,v4); SORT2(v4,v5); SORT2(v5,v6); \
        SORT2(v0,v1); SORT2(v1,v2); SORT2(v2,v3); SORT2(v3,v4); SORT2(v4,v5); \
        SORT2(v0,v1); SORT2(v1,v2); SORT2(v2,v3); SORT2(v3,v4); \
        SORT2(v0,v1); SORT2(v1,v2); SORT2(v2,v3); \
        SORT2(v0,v1); SORT2(v1,v2); \
        SORT2(v0,v1); \
    } while(0)

    #define BUBBLE_SORT_11(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10) do { \
        SORT2(c0,c1); SORT2(c1,c2); SORT2(c2,c3); SORT2(c3,c4); SORT2(c4,c5); SORT2(c5,c6); SORT2(c6,c7); SORT2(c7,c8); SORT2(c8,c9); SORT2(c9,c10); \
        SORT2(c0,c1); SORT2(c1,c2); SORT2(c2,c3); SORT2(c3,c4); SORT2(c4,c5); SORT2(c5,c6); SORT2(c6,c7); SORT2(c7,c8); SORT2(c8,c9); \
        SORT2(c0,c1); SORT2(c1,c2); SORT2(c2,c3); SORT2(c3,c4); SORT2(c4,c5); SORT2(c5,c6); SORT2(c6,c7); SORT2(c7,c8); \
        SORT2(c0,c1); SORT2(c1,c2); SORT2(c2,c3); SORT2(c3,c4); SORT2(c4,c5); SORT2(c5,c6); SORT2(c6,c7); \
        SORT2(c0,c1); SORT2(c1,c2); SORT2(c2,c3); SORT2(c3,c4); SORT2(c4,c5); SORT2(c5,c6); \
        SORT2(c0,c1); SORT2(c1,c2); SORT2(c2,c3); SORT2(c3,c4); SORT2(c4,c5); \
        SORT2(c0,c1); SORT2(c1,c2); SORT2(c2,c3); SORT2(c3,c4); \
        SORT2(c0,c1); SORT2(c1,c2); SORT2(c2,c3); \
        SORT2(c0,c1); SORT2(c1,c2); \
        SORT2(c0,c1); \
    } while(0)

    void MedianFilter7x7(const Image<uint8_t>& input, Image<uint8_t>& output)
    {
        int width = input.Width();
        int height = input.Height();

        for (int y = 3; y < height - 3; ++y)
        {
            int x = 3;
            int remaining = width - 6;

            while (remaining > 0)
            {
                size_t vl = __riscv_vsetvl_e8m1(remaining);
                
                vuint8m1_t v00, v01, v02, v03, v04, v05, v06;
                vuint8m1_t v10, v11, v12, v13, v14, v15, v16;
                vuint8m1_t v20, v21, v22, v23, v24, v25, v26;
                vuint8m1_t v30, v31, v32, v33, v34, v35, v36;
                vuint8m1_t v40, v41, v42, v43, v44, v45, v46;
                vuint8m1_t v50, v51, v52, v53, v54, v55, v56;
                vuint8m1_t v60, v61, v62, v63, v64, v65, v66;

                const uint8_t* p0 = input.GetPtr(0, y - 3) + x;
                v00 = __riscv_vle8_v_u8m1(p0 - 3, vl); v01 = __riscv_vle8_v_u8m1(p0 - 2, vl); v02 = __riscv_vle8_v_u8m1(p0 - 1, vl); v03 = __riscv_vle8_v_u8m1(p0, vl); v04 = __riscv_vle8_v_u8m1(p0 + 1, vl); v05 = __riscv_vle8_v_u8m1(p0 + 2, vl); v06 = __riscv_vle8_v_u8m1(p0 + 3, vl);
                
                const uint8_t* p1 = input.GetPtr(0, y - 2) + x;
                v10 = __riscv_vle8_v_u8m1(p1 - 3, vl); v11 = __riscv_vle8_v_u8m1(p1 - 2, vl); v12 = __riscv_vle8_v_u8m1(p1 - 1, vl); v13 = __riscv_vle8_v_u8m1(p1, vl); v14 = __riscv_vle8_v_u8m1(p1 + 1, vl); v15 = __riscv_vle8_v_u8m1(p1 + 2, vl); v16 = __riscv_vle8_v_u8m1(p1 + 3, vl);
                
                const uint8_t* p2 = input.GetPtr(0, y - 1) + x;
                v20 = __riscv_vle8_v_u8m1(p2 - 3, vl); v21 = __riscv_vle8_v_u8m1(p2 - 2, vl); v22 = __riscv_vle8_v_u8m1(p2 - 1, vl); v23 = __riscv_vle8_v_u8m1(p2, vl); v24 = __riscv_vle8_v_u8m1(p2 + 1, vl); v25 = __riscv_vle8_v_u8m1(p2 + 2, vl); v26 = __riscv_vle8_v_u8m1(p2 + 3, vl);

                const uint8_t* p3 = input.GetPtr(0, y) + x;
                v30 = __riscv_vle8_v_u8m1(p3 - 3, vl); v31 = __riscv_vle8_v_u8m1(p3 - 2, vl); v32 = __riscv_vle8_v_u8m1(p3 - 1, vl); v33 = __riscv_vle8_v_u8m1(p3, vl); v34 = __riscv_vle8_v_u8m1(p3 + 1, vl); v35 = __riscv_vle8_v_u8m1(p3 + 2, vl); v36 = __riscv_vle8_v_u8m1(p3 + 3, vl);

                const uint8_t* p4 = input.GetPtr(0, y + 1) + x;
                v40 = __riscv_vle8_v_u8m1(p4 - 3, vl); v41 = __riscv_vle8_v_u8m1(p4 - 2, vl); v42 = __riscv_vle8_v_u8m1(p4 - 1, vl); v43 = __riscv_vle8_v_u8m1(p4, vl); v44 = __riscv_vle8_v_u8m1(p4 + 1, vl); v45 = __riscv_vle8_v_u8m1(p4 + 2, vl); v46 = __riscv_vle8_v_u8m1(p4 + 3, vl);

                const uint8_t* p5 = input.GetPtr(0, y + 2) + x;
                v50 = __riscv_vle8_v_u8m1(p5 - 3, vl); v51 = __riscv_vle8_v_u8m1(p5 - 2, vl); v52 = __riscv_vle8_v_u8m1(p5 - 1, vl); v53 = __riscv_vle8_v_u8m1(p5, vl); v54 = __riscv_vle8_v_u8m1(p5 + 1, vl); v55 = __riscv_vle8_v_u8m1(p5 + 2, vl); v56 = __riscv_vle8_v_u8m1(p5 + 3, vl);

                const uint8_t* p6 = input.GetPtr(0, y + 3) + x;
                v60 = __riscv_vle8_v_u8m1(p6 - 3, vl); v61 = __riscv_vle8_v_u8m1(p6 - 2, vl); v62 = __riscv_vle8_v_u8m1(p6 - 1, vl); v63 = __riscv_vle8_v_u8m1(p6, vl); v64 = __riscv_vle8_v_u8m1(p6 + 1, vl); v65 = __riscv_vle8_v_u8m1(p6 + 2, vl); v66 = __riscv_vle8_v_u8m1(p6 + 3, vl);

                BUBBLE_SORT_7(v00, v10, v20, v30, v40, v50, v60);
                BUBBLE_SORT_7(v01, v11, v21, v31, v41, v51, v61);
                BUBBLE_SORT_7(v02, v12, v22, v32, v42, v52, v62);
                BUBBLE_SORT_7(v03, v13, v23, v33, v43, v53, v63);
                BUBBLE_SORT_7(v04, v14, v24, v34, v44, v54, v64);
                BUBBLE_SORT_7(v05, v15, v25, v35, v45, v55, v65);
                BUBBLE_SORT_7(v06, v16, v26, v36, v46, v56, v66);

                BUBBLE_SORT_7(v00, v01, v02, v03, v04, v05, v06);
                BUBBLE_SORT_7(v10, v11, v12, v13, v14, v15, v16);
                BUBBLE_SORT_7(v20, v21, v22, v23, v24, v25, v26);
                BUBBLE_SORT_7(v30, v31, v32, v33, v34, v35, v36);
                BUBBLE_SORT_7(v40, v41, v42, v43, v44, v45, v46);
                BUBBLE_SORT_7(v50, v51, v52, v53, v54, v55, v56);
                BUBBLE_SORT_7(v60, v61, v62, v63, v64, v65, v66);

                vuint8m1_t d0 = __riscv_vmv_v_x_u8m1(0, vl);
                vuint8m1_t d1 = __riscv_vmv_v_x_u8m1(0, vl);
                vuint8m1_t d2 = __riscv_vmv_v_x_u8m1(0, vl);
                vuint8m1_t d3 = __riscv_vmv_v_x_u8m1(0, vl);
                vuint8m1_t d4 = __riscv_vmv_v_x_u8m1(0, vl);
                vuint8m1_t d5 = __riscv_vmv_v_x_u8m1(0, vl);

                BUBBLE_SORT_7(d2,  d0,  v26, v16, v06, v05, v04);
                BUBBLE_SORT_7(d3,  v50, v35, v25, v15, v14, v13);
                BUBBLE_SORT_7(v60, v51, v44, v34, v24, v23, v22);
                BUBBLE_SORT_7(d4,  v61, v53, v43, v33, v32, v31);
                BUBBLE_SORT_7(d5,  d1,  v62, v52, v42, v41, v40);

                BUBBLE_SORT_11(v04, v05, v14, v23, v24, v33, v42, v43, v52, v61, v62);

                __riscv_vse8_v_u8m1(output.GetPtr(0, y) + x, v33, vl);

                x += vl;
                remaining -= vl;
            }
        }
    }
}