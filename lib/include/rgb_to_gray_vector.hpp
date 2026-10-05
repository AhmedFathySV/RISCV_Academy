#ifndef RGB_TO_GRAY_VECTOR_H
#define RGB_TO_GRAY_VECTOR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void riscv_enable_vector(void);
void converter_rvv(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* gray, size_t size);

#ifdef __cplusplus
}
#endif

#endif // RGB_TO_GRAY_VECTOR_H