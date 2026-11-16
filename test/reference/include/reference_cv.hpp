#pragma once

#include "image_Add.hpp"
#include "image.hpp"

namespace ref {

template <typename T>
void RGBtoGray(const Image<T, ImageType::RGB>& input,
               Image<T>& output,
               OverFlowPolicy policy = OverFlowPolicy::WRAP)
{
    (void)policy;
    assert(input.Width() == output.Width());
    assert(input.Height() == output.Height());

    const int width = input.Width();
    const int height = input.Height();
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const T r = input.GetPixel(x, y, 0);
            const T g = input.GetPixel(x, y, 1);
            const T b = input.GetPixel(x, y, 2);
            const T gray = static_cast<T>((77 * r + 150 * g + 29 * b) >> 8);
            output.SetPixel(x, y, gray);
        }
    }
}

} // namespace ref
