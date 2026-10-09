#include "gtest_lite.hpp"
#include "RGB_toGrey.hpp"

// Integer-based reference function (equivalent to round(0.299*R + 0.587*G + 0.114*B))
static inline uint8_t ReferenceRGBToGreyInteger(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t val = 299U * r + 587U * g + 114U * b + 500U;
    return static_cast<uint8_t>(val / 1000U);
}

static void FillImage(Image<uint8_t, ImageType::RGB>& img, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t* ptr = img.GetPtr(0, 0);
    const uint32_t totalPixels = img.Width() * img.Height();
    for (uint32_t i = 0; i < totalPixels; ++i) {
        ptr[i * 3 + 0] = r;
        ptr[i * 3 + 1] = g;
        ptr[i * 3 + 2] = b;
    }
}

TEST(RGBToGreyTest, CornerCases_BlackAndWhite) {
    const uint32_t w = 2, h = 2;
    Image<uint8_t, ImageType::RGB> input(w, h);
    Image<uint8_t, ImageType::GRAY> output(w, h);

    // Black test
    FillImage(input, 0, 0, 0);
    ref::RGB_Grey_trans(input, output);
    const uint8_t* outPtr = output.GetPtr(0, 0);
    for (uint32_t i = 0; i < w * h; ++i) {
        EXPECT_EQ(outPtr[i], 0);
    }

    // White test
    FillImage(input, 255, 255, 255);
    ref::RGB_Grey_trans(input, output);
    for (uint32_t i = 0; i < w * h; ++i) {
        EXPECT_EQ(outPtr[i], 255);
    }
}

TEST(RGBToGreyTest, PrimaryColors) {
    Image<uint8_t, ImageType::RGB> input(1, 1);
    Image<uint8_t, ImageType::GRAY> output(1, 1);
    uint8_t* inPtr = input.GetPtr(0, 0);

    // Red
    inPtr[0] = 255; inPtr[1] = 0; inPtr[2] = 0;
    ref::RGB_Grey_trans(input, output);
    EXPECT_EQ(output.GetPtr(0, 0)[0], 76);

    // Green
    inPtr[0] = 0; inPtr[1] = 255; inPtr[2] = 0;
    ref::RGB_Grey_trans(input, output);
    EXPECT_EQ(output.GetPtr(0, 0)[0], 150);

    // Blue
    inPtr[0] = 0; inPtr[1] = 0; inPtr[2] = 255;
    ref::RGB_Grey_trans(input, output);
    EXPECT_EQ(output.GetPtr(0, 0)[0], 29);
}

TEST(RGBToGreyTest, PrecisionVsFloatReference) {
    const uint32_t w = 4, h = 4;
    Image<uint8_t, ImageType::RGB> input(w, h);
    Image<uint8_t, ImageType::GRAY> output(w, h);

    // Bare-metal LCG pseudo-random sequence
    uint32_t seed = 42;
    uint8_t* inPtr = input.GetPtr(0, 0);
    for (uint32_t i = 0; i < w * h * 3; ++i) {
        seed = seed * 1664525U + 1013904223U;
        inPtr[i] = static_cast<uint8_t>(seed >> 24);
    }

    ref::RGB_Grey_trans(input, output);
    const uint8_t* outPtr = output.GetPtr(0, 0);

    for (uint32_t i = 0; i < w * h; ++i) {
        uint8_t r = inPtr[i * 3 + 0];
        uint8_t g = inPtr[i * 3 + 1];
        uint8_t b = inPtr[i * 3 + 2];
        uint8_t expected = ReferenceRGBToGreyInteger(r, g, b);
        uint8_t actual = outPtr[i];

        int diff = static_cast<int>(actual) - static_cast<int>(expected);
        if (diff < 0) diff = -diff;

        EXPECT_TRUE(diff <= 1);
    }
}

TEST(RGBToGreyTest, NonPowerOfTwoDimensions) {
    const uint32_t w = 3, h = 5;
    Image<uint8_t, ImageType::RGB> input(w, h);
    Image<uint8_t, ImageType::GRAY> output(w, h);

    FillImage(input, 100, 150, 200);
    ref::RGB_Grey_trans(input, output);

    const uint8_t* outPtr = output.GetPtr(0, 0);
    uint8_t expected = ReferenceRGBToGreyInteger(100, 150, 200);
    for (uint32_t i = 0; i < w * h; ++i) {
        EXPECT_EQ(outPtr[i], expected);
    }
}