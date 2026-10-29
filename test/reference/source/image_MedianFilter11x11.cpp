#include "image_MedianFilter11x11.hpp"
#include <algorithm>
#include <array>

namespace ref 
{
    void MedianFilter11x11(const Image<uint8_t>& input, Image<uint8_t>& output)
    {
        int width = input.Width();
        int height = input.Height();

        for (int y = 5; y < height - 5; ++y)
        {
            for (int x = 5; x < width - 5; ++x)
            {
                std::array<uint8_t, 121> window;
                int k = 0;
                
                for (int dy = -5; dy <= 5; ++dy)
                {
                    for (int dx = -5; dx <= 5; ++dx)
                    {
                        window[k++] = input.GetPixel(x + dx, y + dy);
                    }
                }
                
                std::nth_element(window.begin(), window.begin() + 60, window.end());
                output.SetPixel(x, y, window[60]);
            }
        }
    }
}