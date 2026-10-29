#include "image_MedianFilter5x5.hpp"
#include <algorithm>
#include <array>

namespace ref 
{
    void MedianFilter5x5(const Image<uint8_t>& input, Image<uint8_t>& output)
    {
        int width = input.Width();
        int height = input.Height();

        for (int y = 2; y < height - 2; ++y)
        {
            for (int x = 2; x < width - 2; ++x)
            {
                std::array<uint8_t, 25> window;
                int k = 0;
                
                for (int dy = -2; dy <= 2; ++dy)
                {
                    for (int dx = -2; dx <= 2; ++dx)
                    {
                        window[k++] = input.GetPixel(x + dx, y + dy);
                    }
                }
                
                std::nth_element(window.begin(), window.begin() + 12, window.end());
                output.SetPixel(x, y, window[12]);
            }
        }
    }
}