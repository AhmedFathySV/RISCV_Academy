// #include "reference_cv.hpp" // Provides Image, RGB, and GRAY
#include "image.hpp"

namespace vec {

    void RGB_toGrey_vectorized(const Image<uint8_t, ImageType::RGB>& RGBImage, 
                                Image<uint8_t, ImageType::GRAY>& output); 

}