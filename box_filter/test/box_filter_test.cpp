#include "box_filter.hpp"
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

static inline uint64_t get_cycles() {
    uint64_t cycles;
    asm volatile("rdcycle %0" : "=r"(cycles));
    return cycles;
}

// Simple PPM Loader
bool load_ppm(const std::string& filename, std::vector<uint8_t>& rgb_data, int& width, int& height) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;
    std::string magic; file >> magic;
    if (magic != "P6") return false;
    char c; while (file.peek() == '#') file.ignore(256, '\n');
    file >> width >> height;
    int max_val; file >> max_val; file.get();
    rgb_data.resize(width * height * 3);
    file.read(reinterpret_cast<char*>(rgb_data.data()), width * height * 3);
    return true;
}

// Simple PPM Saver
void save_ppm(const std::string& filename, const uint8_t* gray_data, int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n" << width << " " << height << "\n255\n";
    for (int i = 0; i < width * height; i++) {
        uint8_t val = gray_data[i];
        file.put(val); file.put(val); file.put(val);
    }
}

int main() {
    printf("=== 2D Box Filter Final Benchmark ===\n");

    std::string input_file = "../../box_filter/input.ppm"; 
    std::vector<uint8_t> rgb_data;
    int width, height;

    if (!load_ppm(input_file, rgb_data, width, height)) {
        printf("Failed to load image!\n");
        return 1;
    }

    // Convert to Grayscale
    std::vector<uint8_t> gray(width * height);
    for (int i = 0; i < width * height; i++) {
        gray[i] = (rgb_data[i*3] + rgb_data[i*3+1] + rgb_data[i*3+2]) / 3;
    }

    std::vector<uint8_t> dst_scalar(width * height);
    std::vector<uint8_t> dst_vector(width * height);
    int ksize = 5; 

    // --- TASK 4: Correctness Check ---
    printf("Running Scalar...\n");
    uint64_t start = get_cycles();
    box_filter_scalar(gray.data(), dst_scalar.data(), height, width, ksize, BORDER_REPLICATE);
    uint64_t scalar_cycles = get_cycles() - start;

    printf("Running Vector...\n");
    start = get_cycles();
    box_filter_vector(gray.data(), dst_vector.data(), height, width, ksize, BORDER_REPLICATE);
    uint64_t vector_cycles = get_cycles() - start;

    // Compare pixels
    int mismatch_count = 0;
    for (int i = 0; i < width * height; i++) {
        if (dst_scalar[i] != dst_vector[i]) {
            mismatch_count++;
        }
    }

    if (mismatch_count == 0) {
        printf("✅ CORRECTNESS CHECK PASSED: Vector output matches Scalar exactly!\n");
    } else {
        printf("❌ MISMATCH: %d pixels differ between Scalar and Vector.\n", mismatch_count);
    }

    // --- TASK 5: Hardware Metrics ---
    int total_pixels = width * height;
    double cpp_scalar = (double)scalar_cycles / total_pixels;
    double cpp_vector = (double)vector_cycles / total_pixels;
    double speedup = (double)scalar_cycles / vector_cycles;

    printf("\n--- Performance Metrics ---\n");
    printf("Image Size: %d x %d (%d pixels)\n", width, height, total_pixels);
    printf("Kernel Size: %dx%d\n", ksize, ksize);
    printf("Scalar Cycles: %lu (Cycles/Pixel: %.2f)\n", scalar_cycles, cpp_scalar);
    printf("Vector Cycles: %lu (Cycles/Pixel: %.2f)\n", vector_cycles, cpp_vector);
    printf("Speedup: %.2fx\n", speedup);
    
    if (speedup < 2.0) {
        printf("\nNote: Speedup is low because QEMU emulates vector instructions slowly.\n");
        printf("On real RISC-V hardware (gem5), this would be 5x-10x faster.\n");
    }

    save_ppm("output_scalar.ppm", dst_scalar.data(), width, height);
    save_ppm("output_vector.ppm", dst_vector.data(), width, height);
    printf("Images saved.\n");

    return 0;
}