#include "gtest_lite.hpp"
#include "reference_cv.hpp"
#include "riscv_cv.hpp"
#include <cstdlib>

TEST(MedianFilter5x5, OutputMatchesScalar) {
    int width = 128;
    int height = 128;
    
    Image<uint8_t> input(width, height);
    Image<uint8_t> output_ref(width, height);
    Image<uint8_t> output_vec(width, height);

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            input.SetPixel(x, y, rand() % 256);
            output_ref.SetPixel(x, y, 0);
            output_vec.SetPixel(x, y, 0);
        }
    }

    ref::MedianFilter5x5(input, output_ref);
    vec::MedianFilter5x5(input, output_vec);

    for(int y = 2; y < height - 2; ++y) {
        for(int x = 2; x < width - 2; ++x) {
            ASSERT_EQ(output_ref.GetPixel(x, y), output_vec.GetPixel(x, y));
        }
    }
}