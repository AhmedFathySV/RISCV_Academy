#include "image_MedianFilter3x3.hpp"
#include <algorithm>
#include <array>

namespace ref 
{
    void MedianFilter3x3(const Image<uint8_t>& input, Image<uint8_t>& output)
    {
        int width = input.Width();
        int height = input.Height();

        for (int y = 1; y < height - 1; ++y)
        {
            for (int x = 1; x < width - 1; ++x)
            {
                std::array<uint8_t, 9> window;
                int k = 0;
                
                for (int dy = -1; dy <= 1; ++dy)
                {
                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        window[k++] = input.GetPixel(x + dx, y + dy);
                    }
                }
                
                std::nth_element(window.begin(), window.begin() + 4, window.end());
                output.SetPixel(x, y, window[4]);
            }
        }
    }
}