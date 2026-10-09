# include "RGB_toGrey.hpp"

// we need to implement interleaved vs planar way of saving images
// we need to implement fixed point vs floating point 
// we need to implement different overflow policies

    // constexpr uint16_t RedConst   = uint16_t(0.299f * (1<<16));  // Q0.16
    // constexpr uint16_t GreenConst = uint16_t(0.587f * (1<<16));
    // constexpr uint16_t BlueConst  = uint16_t(0.114f * (1<<16));
    //constexpr uint16_t RoundNear  = uint16_t(0.5f   * (1<<16)); // I have learnt we need to add 0.5 

    constexpr uint32_t RedConst = 19595; // 0.299 * 65536
    constexpr uint32_t GreenConst = 38470; // 0.587 * 65536
    constexpr uint32_t BlueConst = 7471;  // 0.114 * 65536
    constexpr uint32_t RoundNear = 32768; // 0.5   * 65536

void ref::RGB_Grey_trans(const Image<uint8_t, ImageType::RGB>& RGBImage, Image<uint8_t, ImageType::GRAY>& output){

    const uint32_t PixelNum = RGBImage.Width() *  RGBImage.Height();
    const uint8_t* InPtr    = RGBImage.GetPtr(0,0); 
    uint8_t* OutPtr         = output.GetPtr(0,0); 
    uint32_t Container; 

    for (int i = 0; i < PixelNum; i++) {
        Container    =  ( InPtr[i*3]*RedConst + InPtr[i*3 + 1]*GreenConst + InPtr[i*3 + 2]*BlueConst + RoundNear); // interleaving
        OutPtr[i]    =  uint8_t(Container >> 16);
    }
 
}

