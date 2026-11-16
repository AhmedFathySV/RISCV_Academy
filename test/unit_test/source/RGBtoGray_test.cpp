#include "gtest_lite.hpp"
#include "riscv_cv.hpp"
#include "reference_cv.hpp"
#include "test_utils.hpp"

// Include the generated nature image header
#include "nature_lake.hpp"

class RGBtoGrayTest : public ::testing::TestWithParam<std::tuple<int, int>>
{
};

// Test 1: Parameterized test with random data to ensure algorithm correctness
TEST_P(RGBtoGrayTest, VectorizedMatchesReference)
{
    const auto [width, height] = GetParam();
    
    Image<uint8_t, ImageType::RGB> input_rgb(width, height);
    Image<uint8_t> output_reference(width, height);
    Image<uint8_t> output_vectorized(width, height);
    
    RandomInt<uint8_t> random(0, 255);
    random.ImageRandomInitialize(input_rgb);
    
    ref::RGBtoGray(input_rgb, output_reference);
    vec::RGBtoGray(input_rgb, output_vectorized);
    
    ExpectImagesEqual(output_reference, output_vectorized);
}

INSTANTIATE_TEST_SUITE_P(
    ImageSizes,
    RGBtoGrayTest,
    ::testing::Combine(
        ::testing::Values(16, 32, 64),
        ::testing::Values(16, 32, 64)
    )
);

// Test 2: Actual test using the embedded nature image
TEST(RGBtoGrayTest, EmbeddedNatureImageConversion)
{
    printf("Testing with embedded nature image: %dx%d, Channels: %d\n",
           nature_lake_width, nature_lake_height, nature_lake_channels);

    // QEMU builds use hosted file I/O; bare-metal gem5 builds use the embedded buffer API.
    Image<uint8_t, ImageType::RGB> input_rgb;
#ifdef RISCV_QEMU
    input_rgb.Read("/home/RISCV_Academy/images/nature_lake.ppm");
#else
    input_rgb.Read(nature_lake_width, nature_lake_height, nature_lake_data);
#endif

    Image<uint8_t> output_reference(nature_lake_width, nature_lake_height);
    Image<uint8_t> output_vectorized(nature_lake_width, nature_lake_height);

    ref::RGBtoGray(input_rgb, output_reference);
    vec::RGBtoGray(input_rgb, output_vectorized);

    ExpectImagesEqual(output_reference, output_vectorized);

    printf("SUCCESS: RGB to Gray conversion matched for %dx%d image!\n",
           nature_lake_width, nature_lake_height);
}
