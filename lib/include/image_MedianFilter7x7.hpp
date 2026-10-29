#pragma once
#include "types.hpp"

namespace vec 
{
    void MedianFilter7x7(const Image<uint8_t>& input, Image<uint8_t>& output);
}