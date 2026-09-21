#include "riscv_cv.hpp"

#ifndef RISCV_QEMU
#include "image0.hpp"
#include "image1.hpp"
#endif

int main()
{
#ifdef RISCV_BAREMETAL
    riscv_enable_vector();
#endif

    // Read two input images
    Image<uint8_t> image0, image1;

#ifdef RISCV_QEMU
    image0.Read("/home/RISCV_Academy/images/image0.pgm");
    image1.Read("/home/RISCV_Academy/images/image1.pgm");
#else
    image0.Read(image0_width, image0_height, image0_data);
    image1.Read(image1_width, image1_height, image1_data);
#endif

    // Add with saturation
    Image<uint8_t> out(image0.Width(), image0.Height());

    Timer timer;

    timer.Start();
    vec::Add(image0, image1, out, OverFlowPolicy::SATURATE);
    timer.Stop();

    printf("Time: %llu cycles, %llu instructions\n",
           (unsigned long long)timer.ElapsedCycles(),
           (unsigned long long)timer.ElapsedInstructions());

    // Write output
#ifdef RISCV_QEMU
    out.Write("/home/RISCV_Academy/images/output.pgm");
    printf("Wrote output.pgm (%dx%d)\n", out.Width(), out.Height());
#endif

    printf("Example passed.\n");
    return 0;
}
