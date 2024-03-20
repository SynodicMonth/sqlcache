#pragma once

#include "benchmark.h"
#include "jdbc/connector.h"
#include "utils/zipfian.h"
#include "utils/randstring.h"
#include "cache/AutoIncCache.h"
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

    bool read_record(int key, std::vector<std::string>& results);
    bool insert_record(int key, std::vector<std::string>& values);

    void enable_cache();
    
    std::string generate_read_sql(int key);
    std::string generate_insert_sql(int key, std::vector<std::string>& values);
public:

    std::unique_ptr<sql::Statement> stmt;
    std::unique_ptr<sql::PreparedStatement> insert_stmt;
    std::default_random_engine generator;
    zipfian_int_distribution<int> zipfian_distrib;
    unsigned int next_key = 0;
    std::random_device rd;
    std::mt19937 rng;

    unsigned int max_records = 1000;
    unsigned int max_fields = 10;
    unsigned int max_field_size = 100; // in chars
    std::string table_name = "usertable";
    unsigned int max_query = 10000;
    unsigned int current_query = 0;

    double query_proportion = 1.0;
    double insert_proportion = 0.0;
    bool cache_enabled = false;
};