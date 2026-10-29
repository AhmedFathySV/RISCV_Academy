#pragma once
#include "types.hpp"

namespace ref 
{
    void MedianFilter3x3(const Image<uint8_t>& input, Image<uint8_t>& output);
}