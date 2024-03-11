#include "benchmark.h"

bool Benchmark::is_end() {
    return true;
}

bool Benchmark::run() {
    create_database();
    load_database();
    auto start = std::chrono::high_resolution_clock::now();
    while (!is_end()) {
        bool ret = step();
        if (!ret) {
            std::cout << "Benchmark: step failed" << std::endl;
            return false;
        } 
        records_executed++;
    }
    auto end = std::chrono::high_resolution_clock::now();
    millis_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cleanup();
    return true;
}