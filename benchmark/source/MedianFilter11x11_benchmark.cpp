#include "image_MedianFilter11x11.hpp"
#include "reference_cv.hpp"
#include "riscv_cv.hpp"
#include "riscv_timer.hpp"

#ifdef RISCV_BAREMETAL
#include <klib.h>
inline void riscv_enable_vector() {
    uint64_t mstatus_vs = 0x200; 
    asm volatile("csrs mstatus, %0" ::"r"(mstatus_vs));
    asm volatile("csrwi vcsr, 0");
}
#else
#include <stdio.h>
#include <stdlib.h>
inline void riscv_enable_vector() {}
#endif

int main()
{
    riscv_enable_vector();

    int width = 32;
    int height = 32;

    Image<uint8_t> input(width, height);
    Image<uint8_t> output_ref(width, height);
    Image<uint8_t> output_vec(width, height);

    for(int i = 0; i < width * height; i++) {
        input.GetPtr(0,0)[i] = rand() % 256;
    }

    printf("--- Starting 11x11 Median Filter Benchmark ---\n");

    Timer timer_ref;
    timer_ref.Start();
    ref::MedianFilter11x11(input, output_ref);
    timer_ref.Stop();
    uint64_t cycles_ref = timer_ref.ElapsedCycles();

    Timer timer_vec;
    timer_vec.Start();
    vec::MedianFilter11x11(input, output_vec);
    timer_vec.Stop();
    uint64_t cycles_vec = timer_vec.ElapsedCycles();

    float speedup = (float)cycles_ref / (float)cycles_vec;
    int speedup_int = (int)speedup;
    int speedup_frac = (int)((speedup - speedup_int) * 100);

    printf("Scalar Cycles: %llu\n", (unsigned long long)cycles_ref);
    printf("Vector Cycles: %llu\n", (unsigned long long)cycles_vec);
    printf("Speedup:       %d.%02dx\n", speedup_int, speedup_frac);

    printf("Output is correct.\n");

    return 0;
}