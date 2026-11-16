#include "benchmark.hpp"
#include "riscv_port.hpp"

#include <riscv_vector.h>

#include <cstddef>
#include <cstdint>

namespace
{
constexpr size_t SampleCount = 10000;
constexpr size_t BinCount = 256;
constexpr size_t MaxVectorLanes = 256;

uint8_t samples[SampleCount];
uint32_t scalarHistogram[BinCount];
uint32_t vectorHistogram[BinCount];
uint32_t privateHistograms[MaxVectorLanes][BinCount];

void generateConstant(uint8_t* data, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        data[i] = 128;
}

void generateUniform(uint8_t* data, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        data[i] = static_cast<uint8_t>(rand() & 0xff);
}

void generateSkewed(uint8_t* data, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        uint8_t value = 0xff;
        for (int sample = 0; sample < 4; ++sample)
        {
            const uint8_t candidate = static_cast<uint8_t>(rand() & 0xff);
            if (candidate < value)
                value = candidate;
        }
        data[i] = value;
    }
}

void calculateScalar(const uint8_t* data, size_t count, uint32_t* histogram)
{
    for (size_t bin = 0; bin < BinCount; ++bin)
        histogram[bin] = 0;
    for (size_t i = 0; i < count; ++i)
        ++histogram[data[i]];
}

bool calculateRvv(const uint8_t* data, size_t count, uint32_t* histogram)
{
    const size_t laneCount = __riscv_vsetvlmax_e8m1();
    if (laneCount > MaxVectorLanes)
        return false;

    for (size_t lane = 0; lane < laneCount; ++lane)
        for (size_t bin = 0; bin < BinCount; ++bin)
            privateHistograms[lane][bin] = 0;

    for (size_t offset = 0; offset < count;)
    {
        const size_t vl = __riscv_vsetvl_e8m1(count - offset);
        const vuint8m1_t pixels = __riscv_vle8_v_u8m1(data + offset, vl);
        const vuint32m4_t bins = __riscv_vzext_vf4_u32m4(pixels, vl);
        const vuint32m4_t lanes = __riscv_vid_v_u32m4(vl);
        const vuint32m4_t rowOffsets =
            __riscv_vmul_vx_u32m4(lanes, BinCount * sizeof(uint32_t), vl);
        const vuint32m4_t binOffsets = __riscv_vsll_vx_u32m4(bins, 2, vl);
        const vuint32m4_t offsets =
            __riscv_vadd_vv_u32m4(rowOffsets, binOffsets, vl);

        const vuint32m4_t oldCounts =
            __riscv_vluxei32_v_u32m4(&privateHistograms[0][0], offsets, vl);
        const vuint32m4_t newCounts = __riscv_vadd_vx_u32m4(oldCounts, 1, vl);
        __riscv_vsuxei32_v_u32m4(&privateHistograms[0][0], offsets, newCounts, vl);
        offset += vl;
    }

    for (size_t bin = 0; bin < BinCount; ++bin)
        histogram[bin] = 0;
    for (size_t lane = 0; lane < laneCount; ++lane)
        for (size_t bin = 0; bin < BinCount; ++bin)
            histogram[bin] += privateHistograms[lane][bin];

    return true;
}

bool runDataset(const char* name, void (*generate)(uint8_t*, size_t))
{
    generate(samples, SampleCount);

    Timer scalarTimer;
    scalarTimer.Start();
    calculateScalar(samples, SampleCount, scalarHistogram);
    scalarTimer.Stop();

    Timer vectorTimer;
    vectorTimer.Start();
    const bool vectorSupported = calculateRvv(samples, SampleCount, vectorHistogram);
    vectorTimer.Stop();

    if (!vectorSupported)
    {
        printf("RVV lane count exceeds the private histogram capacity.\n");
        return false;
    }

    bool matches = true;
    uint64_t total = 0;
    for (size_t bin = 0; bin < BinCount; ++bin)
    {
        total += vectorHistogram[bin];
        if (scalarHistogram[bin] != vectorHistogram[bin])
            matches = false;
    }

    printf("========================================\n");
    printf("Dataset: %s (N = %u)\n", name, static_cast<unsigned>(SampleCount));
    printf("  256-bin sum: %s\n", total == SampleCount ? "PASSED" : "FAILED");
    printf("  RVV matches scalar: %s\n", matches ? "PASSED" : "FAILED");
    printf("  Scalar: %llu cycles, %llu instructions\n",
           static_cast<unsigned long long>(scalarTimer.ElapsedCycles()),
           static_cast<unsigned long long>(scalarTimer.ElapsedInstructions()));
    printf("  RVV gather/scatter: %llu cycles, %llu instructions\n",
           static_cast<unsigned long long>(vectorTimer.ElapsedCycles()),
           static_cast<unsigned long long>(vectorTimer.ElapsedInstructions()));

    for (size_t group = 0; group < 16; ++group)
    {
        uint32_t scalarCount = 0;
        uint32_t vectorCount = 0;
        for (size_t bin = group * 16; bin < group * 16 + 16; ++bin)
        {
            scalarCount += scalarHistogram[bin];
            vectorCount += vectorHistogram[bin];
        }
        if (scalarCount != vectorCount)
            matches = false;
        printf("  %3u-%3u: %u\n", static_cast<unsigned>(group * 16),
               static_cast<unsigned>(group * 16 + 15), vectorCount);
    }

    return matches && total == SampleCount;
}
}

int main()
{
    riscv_enable_vector();
    srand(static_cast<unsigned>(ReadCycles()));

    bool allCorrect = true;
    allCorrect = runDataset("Constant", generateConstant) && allCorrect;
    allCorrect = runDataset("Uniform-Random", generateUniform) && allCorrect;
    allCorrect = runDataset("Strongly Skewed", generateSkewed) && allCorrect;

    if (allCorrect)
    {
        printf("Output is correct.\n");
        return 0;
    }

    printf("Output is wrong!\n");
    return 1;
}