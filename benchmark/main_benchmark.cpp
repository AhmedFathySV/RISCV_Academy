extern "C" {
    #include <klib.h>
}

#ifdef USE_GEM5_M5
extern "C" {
    #include <gem5/m5ops.h>
}
#else
    #define m5_reset_stats(a, b)
    #define m5_dump_stats(a, b)
#endif

extern "C" {
    void riscv_enable_vector(void);
    void converter_scalar(const uint8_t* r, const uint8_t* g, const uint8_t* b, uint8_t* gray_out, size_t size);
    void converter_rvv(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* gray, size_t size);
}


#define IMG_SIZE (640 * 480)
uint8_t r[IMG_SIZE];
uint8_t g[IMG_SIZE];
uint8_t b[IMG_SIZE];
uint8_t gray[IMG_SIZE];

int main() {
 
    riscv_enable_vector();

    for (size_t i = 0; i < IMG_SIZE; ++i) {
        r[i] = (uint8_t)((i + 10) % 256);
        g[i] = (uint8_t)((i + 20) % 256);
        b[i] = (uint8_t)((i + 30) % 256);
    }

    printf("[Benchmark] Size: %d pixels\n", IMG_SIZE);


    printf("Running Scalar...\n");
    m5_reset_stats(0, 0);
    converter_scalar(r, g, b, gray, IMG_SIZE);
    m5_dump_stats(0, 0);
    
    printf("Running Vectorized RVV...\n");
    m5_reset_stats(0, 0);
    converter_rvv(r, g, b, gray, IMG_SIZE);
    m5_dump_stats(0, 0);

    
    //printf("[Benchmark] Finished successfully.\n");

    return 0;
}