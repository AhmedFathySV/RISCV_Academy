#ifndef RGB_TO_GRAY_SCALAR_H
#define RGB_TO_GRAY_SCALAR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void converter_scalar(const uint8_t* r, const uint8_t* g, const uint8_t* b, uint8_t* gray_out, size_t size);

#ifdef __cplusplus
}
#endif

#endif // RGB_TO_GRAY_SCALAR_H