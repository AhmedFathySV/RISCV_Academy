#include <stdint.h>
#include <stddef.h>

// Standard 256-bin histogram
void compute_histogram_scalar(const uint8_t* image, size_t n, uint32_t* hist) {
    for(int i = 0; i < 256; i++) hist[i] = 0;
    for (size_t i = 0; i < n; i++) {
        hist[image[i]]++;
    }
}

// 16-BIN HISTOGRAM (Exercise 1 Requirement)
void compute_histogram_16bins(const uint8_t* image, size_t n, uint32_t* hist) {
    // Clear 16 bins
    for(int i = 0; i < 16; i++) {
        hist[i] = 0;
    }
    
    for (size_t i = 0; i < n; i++) {
        // BIN INDEX EXPRESSION: Divide by 16 (or shift right by 4)
        // 256 values / 16 bins = 16 values per bin.
        uint8_t bin_index = image[i] >> 4; 
        hist[bin_index]++;
    }
}