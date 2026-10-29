#pragma once
#include "types.hpp"

namespace ref 
{
    void MedianFilter11x11(const Image<uint8_t>& input, Image<uint8_t>& output);
}