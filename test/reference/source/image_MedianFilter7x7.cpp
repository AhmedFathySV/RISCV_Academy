#include "image_MedianFilter7x7.hpp"
#include <algorithm>
#include <array>

namespace ref 
{
    void MedianFilter7x7(const Image<uint8_t>& input, Image<uint8_t>& output)
    {
        int width = input.Width();
        int height = input.Height();

        // Padding = 3 for 7x7 window
        for (int y = 3; y < height - 3; ++y)
        {
            for (int x = 3; x < width - 3; ++x)
            {
                std::array<uint8_t, 49> window;
                int k = 0;
                
                for (int dy = -3; dy <= 3; ++dy)
                {
                    for (int dx = -3; dx <= 3; ++dx)
                    {
                        window[k++] = input.GetPixel(x + dx, y + dy);
                    }
                }
                
                // Median is at index 24 for 49 elements
                std::nth_element(window.begin(), window.begin() + 24, window.end());
                output.SetPixel(x, y, window[24]);
            }
        }
    }
}