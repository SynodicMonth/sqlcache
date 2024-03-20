#include "common.h"
#include "benchmarks/YCSBBenchmark.h"
#include "benchmarks/TracefileBenchmark.h"

int main() {
    MySQLConnector conn;
    conn.initialize("tcp://127.0.0.1:3306", "root", "zheshimima", "test");
    YCSBBenchmark benchmark(conn);
    benchmark.enable_cache();
    benchmark.run();
    double throughput = (double) benchmark.records_executed / (double) benchmark.millis_elapsed * 1000;
    std::cout << "Throughput: " << throughput << " records/s" << std::endl;
    return 0;
}
