#include "rgb_to_gray_vector.hpp"
#include <stdint.h>
#include <stddef.h>
#include <riscv_vector.h>
//#include <klib.h> 

extern "C" {
   #include <klib.h>
}

__attribute__((noinline, optimize("no-tree-vectorize")))
void riscv_enable_vector()
{
    uint64_t mstatus_vs = 0x200; 
    asm volatile("csrs mstatus, %0" ::"r"(mstatus_vs));
    asm volatile("csrwi vcsr, 0");
}

void converter_rvv (uint8_t* r , uint8_t* g , uint8_t* b , uint8_t* gray , size_t size) {
    for (size_t i = 0; i < size; ) {
        size_t vl = __riscv_vsetvl_e8m4(size - i);

        vuint8m4_t  vr = __riscv_vle8_v_u8m4 (&r[i], vl);
        vuint8m4_t  vg = __riscv_vle8_v_u8m4 (&g[i], vl);
        vuint8m4_t  vb = __riscv_vle8_v_u8m4 (&b[i], vl);



        vuint16m8_t vsum = __riscv_vwmulu_vx_u16m8 (vr, 77, vl);
                    vsum = __riscv_vwmaccu_vx_u16m8 (vsum, 150, vg, vl);
                    vsum = __riscv_vwmaccu_vx_u16m8(vsum, 29, vb, vl);

        vuint16m8_t vgray_16 = __riscv_vsrl_vx_u16m8 (vsum, 8,  vl); 
        vuint8m4_t vgray_8   = __riscv_vncvt_x_x_w_u8m4(vgray_16, vl); 

        __riscv_vse8_v_u8m4(&gray[i], vgray_8, vl);
        i += vl; 
    }
}

#ifndef BENCHMARK
int main (){
    riscv_enable_vector();

    uint8_t r[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    uint8_t g[] = {11, 21, 31, 41, 51, 61, 71, 81, 91};
    uint8_t b[] = {12, 22, 32, 42, 52, 62, 72, 82, 92};
    
    size_t size = sizeof(r) / sizeof(r[0]);
    uint8_t gray[size];
    
    converter_rvv(r, g, b, gray, size);
    
    for (size_t i = 0; i < size; i++) {
        printf("Index %d -> Gray: %d\n", i, gray[i]);
    }
    return 0;
}

#endif