#include "gtest_lite.hpp"
#include "reference_cv.hpp"
#include "riscv_cv.hpp"
#include <cstdlib>

TEST(MedianFilter9x9, OutputMatchesScalar) {
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

    ref::MedianFilter9x9(input, output_ref);
    vec::MedianFilter9x9(input, output_vec);

    // Padding for 9x9 is 4 pixels on each side
    for(int y = 4; y < height - 4; ++y) {
        for(int x = 4; x < width - 4; ++x) {
            ASSERT_EQ(output_ref.GetPixel(x, y), output_vec.GetPixel(x, y));
        }
    }
}