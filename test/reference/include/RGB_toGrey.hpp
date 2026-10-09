#ifndef RGB_TO_GREY_HPP
#define RGB_TO_GREY_HPP

#include "reference_cv.hpp" // Provides Image, RGB, and GRAY
#include "image.hpp"

namespace ref {

    void RGB_Grey_trans(const Image<uint8_t, ImageType::RGB>& input, 
                             Image<uint8_t, ImageType::GRAY>& output);



}

#endif // RGB_TO_GREY_HPP