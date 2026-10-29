#pragma once
#include "types.hpp"

namespace vec 
{
    void MedianFilter5x5(const Image<uint8_t>& input, Image<uint8_t>& output);
}