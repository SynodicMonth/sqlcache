#pragma once

#include "benchmark.h"
#include "jdbc/connector.h"
#include "utils/zipfian.h"
#include "utils/randstring.h"
#include <random>
#include <ctime>

class YCSBBenchmark : public Benchmark
{
public:
    YCSBBenchmark(MySQLConnector& conn);
    ~YCSBBenchmark() = default;
    bool create_database();
    bool load_database();
    bool step();
    bool is_end();
    bool cleanup();

    bool read_record(int key);
public:

    std::unique_ptr<sql::PreparedStatement> read_stmt;
    std::default_random_engine generator;
    zipfian_int_distribution<int> distribution;

    unsigned int max_records = 1000;
    unsigned int max_fields = 10;
    unsigned int max_field_size = 100; // in chars
    std::string table_name = "usertable";
    unsigned int max_query = 100000;
    unsigned int current_query = 0;
};