#include "rgb_to_gray.hpp"
#include <stdint.h>
#include <stddef.h>

extern "C" {
   #include <klib.h>
}

__attribute__((optimize("no-tree-vectorize")))
void converter_scalar (const uint8_t* r, const uint8_t* g, const uint8_t* b, uint8_t* gray_out, size_t size) {
    for (size_t i = 0; i < size; i++){
        gray_out[i] = (uint8_t)((77 * r[i] + 150 * g[i] + 29 * b[i]) >> 8);
    }
}

#ifndef BENCHMARK

int main (){

    uint8_t r[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    uint8_t g[] = {11, 21, 31, 41, 51, 61, 71, 81, 91};
    uint8_t b[] = {11, 22, 32, 42, 52, 62, 72, 82, 92};
  
    size_t size = sizeof(r) / sizeof(r[0]);

    uint8_t gray_out[size];

    converter_scalar(r, g, b, gray_out, size);

    for (size_t i = 0; i < size; i++){
        printf("RGB --> R = %d , G = %d , B = %d \nGray representation = %d\n" , 
                    r[i], g[i], b[i], gray_out[i]) ;
        printf("%d, ", gray_out[i]);
    }
    
    return 0;
}

#endif