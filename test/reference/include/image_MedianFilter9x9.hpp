#pragma once
#include "types.hpp"

namespace ref 
{
    void MedianFilter9x9(const Image<uint8_t>& input, Image<uint8_t>& output);
}