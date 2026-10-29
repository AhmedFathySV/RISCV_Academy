#include "image_MedianFilter9x9.hpp"
#include <algorithm>
#include <array>

namespace ref 
{
    void MedianFilter9x9(const Image<uint8_t>& input, Image<uint8_t>& output)
    {
        int width = input.Width();
        int height = input.Height();

        // Padding = 4 for 9x9 window
        for (int y = 4; y < height - 4; ++y)
        {
            for (int x = 4; x < width - 4; ++x)
            {
                std::array<uint8_t, 81> window;
                int k = 0;
                
                for (int dy = -4; dy <= 4; ++dy)
                {
                    for (int dx = -4; dx <= 4; ++dx)
                    {
                        window[k++] = input.GetPixel(x + dx, y + dy);
                    }
                }
                
                // Median is at index 40 for 81 elements
                std::nth_element(window.begin(), window.begin() + 40, window.end());
                output.SetPixel(x, y, window[40]);
            }
        }
    }
}