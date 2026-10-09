#include "reference_cv.hpp"
#include "riscv_cv.hpp"
#include "benchmark.hpp"

// #include <cstdio>

int main()
{

    #ifdef RISCV_BAREMETAL
    // Enable the vector unit before any auto-vectorized loop runs (nexus-am's
    // _start only enables the FPU).
    riscv_enable_vector();
#endif
    // 1. Declare RGB Input Image
    Image<uint8_t, ImageType::RGB> input_image;

#ifdef RISCV_QEMU
    // QEMU mode: Load PPM image directly from disk
    const char* input_path = "../../images/image_reem.ppm";
    printf("Loading image: %s\n", input_path);
    input_image.Read(input_path);
#else
    // Bare-metal mode: Load from embedded C array header
    // Generate header via: xxd -i input.raw > image_data.h
    #include "image1.hpp" 
    input_image.Read(image1_width, image1_height, image1_data);
#endif

    const int width  = input_image.Width();
    const int height = input_image.Height();
    const int loop_count = 3;

    printf("RGB to Grayscale Benchmark: %dx%d, %d iterations\n", width, height, loop_count);

    // 2. Declare Grayscale Output Images
    Image<uint8_t, ImageType::GRAY> output_reference(width, height);
    Image<uint8_t, ImageType::GRAY> output_vectorized(width, height);

    uint64_t reference_cycles = 0, vectorized_cycles = 0;
    uint64_t reference_instrs = 0, vectorized_instrs = 0;

    bool all_correct = true;

    for (int i = 0; i < loop_count; ++i)
    {
        Timer timer_scalar, timer_vectorized;

        // Run Scalar Reference
        timer_scalar.Start();
        ref::RGB_Grey_trans(input_image, output_reference);
        timer_scalar.Stop();

        // Run Vectorized Version
        timer_vectorized.Start();
        vec::RGB_toGrey_vectorized(input_image, output_vectorized);
        timer_vectorized.Stop();

        reference_cycles += timer_scalar.ElapsedCycles();
        reference_instrs += timer_scalar.ElapsedInstructions();

        vectorized_cycles += timer_vectorized.ElapsedCycles();
        vectorized_instrs += timer_vectorized.ElapsedInstructions();

        if (!CheckCorrectness(output_reference, output_vectorized))
        {
            all_correct = false;
            break;
        }
    }

    //  PrintTime(width * height, 
    //            vectorized_cycles / loop_count, vectorized_instrs / loop_count,
    //            reference_cycles / loop_count, reference_instrs / loop_count);
    PrintTime("RGB to Grayscale",
          width * height, 
          vectorized_cycles / loop_count, 
          vectorized_instrs / loop_count,
          reference_cycles / loop_count, 
          reference_instrs / loop_count);

    if (all_correct)
    {
        printf("Output is correct.\n");

#ifdef RISCV_QEMU
        // Write result to PGM image to visually verify output
        output_vectorized.Write("output_gray_reem.pgm");
        printf("Grayscale output saved to output_gray_reem.pgm\n");
#endif
        return 0;
    }
    else
    {
        printf("Output is wrong!\n");
        return 1;
    }

    
}