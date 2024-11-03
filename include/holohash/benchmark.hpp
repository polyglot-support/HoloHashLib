#pragma once
#include "core.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <functional>

namespace holohash {
namespace benchmark {

struct BenchmarkResult {
    std::string name;
    double avg_time_ms;
    double min_time_ms;
    double max_time_ms;
    size_t iterations;
    size_t data_size;
};

template<typename Func>
BenchmarkResult run_benchmark(
    const std::string& name,
    size_t iterations,
    size_t data_size,
    Func&& func
) {
    std::vector<double> timings;
    timings.reserve(iterations);

    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        timings.push_back(duration);
    }

    double total = 0.0;
    double min_time = timings[0];
    double max_time = timings[0];

    for (double time : timings) {
        total += time;
        min_time = std::min(min_time, time);
        max_time = std::max(max_time, time);
    }

    return BenchmarkResult{
        name,
        total / iterations,
        min_time,
        max_time,
        iterations,
        data_size
    };
}

inline void print_result(const BenchmarkResult& result) {
    std::cout << "\nBenchmark: " << result.name << "\n"
              << "Data size: " << result.data_size << " bytes\n"
              << "Iterations: " << result.iterations << "\n"
              << "Average time: " << std::fixed << std::setprecision(3) << result.avg_time_ms << " ms\n"
              << "Min time: " << result.min_time_ms << " ms\n"
              << "Max time: " << result.max_time_ms << " ms\n"
              << "Throughput: " << std::fixed << std::setprecision(2)
              << (result.data_size * 1000.0) / (result.avg_time_ms * 1024 * 1024)
              << " MB/s\n";
}

} // namespace benchmark
} // namespace holohash
