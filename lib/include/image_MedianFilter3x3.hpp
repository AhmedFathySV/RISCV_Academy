#pragma once
#include "types.hpp"

namespace vec 
{
    void MedianFilter3x3(const Image<uint8_t>& input, Image<uint8_t>& output);
}