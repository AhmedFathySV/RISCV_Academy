#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <random>
#include <numeric>
#include <iomanip>
#include <cmath>
#include <string>

// Verify that the sum of histogram bins equals the number of elements
bool verifyHistogram(const std::vector<int>& hist, size_t expectedCount) {
    long long sum = std::accumulate(hist.begin(), hist.end(), 0LL);
    return sum == static_cast<long long>(expectedCount);
}

// Print histogram summaries (optional, for visual verification)
void printHistogramSummary(const std::vector<int>& hist, const std::string& name) {
    std::cout << "  " << name << " (non-zero bins): ";
    int count = 0;
    for (size_t i = 0; i < hist.size(); ++i) {
        if (hist[i] > 0) count++;
    }
    std::cout << count << "\n";
}

void printHistogramChart(const std::vector<int>& hist) {
    const int maxCount = *std::max_element(hist.begin(), hist.end());
    constexpr int chartWidth = 40;

    std::cout << "  16-bin distribution (each bin covers 16 values):\n";
    for (size_t i = 0; i < hist.size(); ++i) {
        const int barLength = maxCount == 0
            ? 0
            : (hist[i] * chartWidth + maxCount - 1) / maxCount;
        const int lower = static_cast<int>(i) * 16;
        const int upper = lower + 15;

        std::cout << "  " << std::setw(3) << lower << "-" << std::setw(3) << upper
                  << " | " << std::string(barLength, '#') << " " << hist[i] << "\n";
    }
}

// Generate constant data
std::vector<uint8_t> generateConstant(size_t n, uint8_t value) {
    return std::vector<uint8_t>(n, value);
}

// Generate uniform-random data
std::vector<uint8_t> generateUniform(size_t n) {
    std::vector<uint8_t> data(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    for (size_t i = 0; i < n; ++i) {
        data[i] = static_cast<uint8_t>(dist(gen));
    }
    return data;
}

// Generate strongly skewed data (exponential-like distribution)
std::vector<uint8_t> generateSkewed(size_t n) {
    std::vector<uint8_t> data(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    // Use a normal distribution centered low to create a strong skew
    std::normal_distribution<double> dist(20.0, 15.0); 
    for (size_t i = 0; i < n; ++i) {
        double val = dist(gen);
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        data[i] = static_cast<uint8_t>(val);
    }
    return data;
}

int main() {
    const size_t N = 10000; // Number of elements

    // 1. Generate the three inputs
    std::vector<uint8_t> dataConst = generateConstant(N, 128);
    std::vector<uint8_t> dataUniform = generateUniform(N);
    std::vector<uint8_t> dataSkewed = generateSkewed(N);

    std::vector<std::pair<std::string, std::vector<uint8_t>>> datasets = {
        {"Constant", dataConst},
        {"Uniform-Random", dataUniform},
        {"Strongly Skewed", dataSkewed}
    };

    for (const auto& [name, data] : datasets) {
        std::cout << "========================================\n";
        std::cout << "Dataset: " << name << " (N = " << data.size() << ")\n";

        // 2. Build 256-bin histogram
        std::vector<int> hist256(256, 0);
        for (uint8_t val : data) {
            hist256[val]++;
        }

        // 3. Verify sum for 256 bins
        if (verifyHistogram(hist256, data.size())) {
            std::cout << "  [256 Bins] Sum check PASSED.\n";
        } else {
            std::cout << "  [256 Bins] Sum check FAILED.\n";
        }

        // 4. Repeat with 16 bins
        std::vector<int> hist16(16, 0);
        for (uint8_t val : data) {
            // Derived bin-index expression: 
            // Since 256 values / 16 bins = 16 values per bin, 
            // the expression is val / 16, or efficiently: val >> 4
            int binIndex = val >> 4; 
            hist16[binIndex]++;
        }


        // Verify sum for 16 bins
        if (verifyHistogram(hist16, data.size())) {
            std::cout << "  [16 Bins]  Sum check PASSED.\n";
        } else {
            std::cout << "  [16 Bins]  Sum check FAILED.\n";
        }

        printHistogramSummary(hist256, "256-bin");
        printHistogramSummary(hist16, "16-bin");
        printHistogramChart(hist16);
    }

    return 0;
}