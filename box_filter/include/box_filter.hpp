#pragma once
#include <cstdint>

// Border types matching OpenCV
#define BORDER_CONSTANT 0
#define BORDER_REPLICATE 1

// Function declarations
void box_filter_scalar(const uint8_t* src, uint8_t* dst, int height, int width, int ksize, int border_type);
void box_filter_vector(const uint8_t* src, uint8_t* dst, int height, int width, int ksize, int border_type);