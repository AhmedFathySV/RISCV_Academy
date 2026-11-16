#include "box_filter.hpp"

void box_filter_scalar(const uint8_t* src, uint8_t* dst, int height, int width, int ksize, int border_type) {
    int radius = ksize / 2;
    int area = ksize * ksize;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int sum = 0;

            // Slide the 2D kernel window
            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    int ny = y + ky;
                    int nx = x + kx;
                    uint8_t val = 0;

                    // Border Handling
                    if (ny < 0 || ny >= height || nx < 0 || nx >= width) {
                        if (border_type == BORDER_CONSTANT) {
                            val = 0; // Pad with zeros
                        } else if (border_type == BORDER_REPLICATE) {
                            // Clamp coordinates to the nearest valid pixel
                            int clamp_y = (ny < 0) ? 0 : (ny >= height ? height - 1 : ny);
                            int clamp_x = (nx < 0) ? 0 : (nx >= width ? width - 1 : nx);
                            val = src[clamp_y * width + clamp_x];
                        }
                    } else {
                        val = src[ny * width + nx];
                    }
                    sum += val;
                }
            }
            // Normalize (average)
            dst[y * width + x] = sum / area;
        }
    }
}