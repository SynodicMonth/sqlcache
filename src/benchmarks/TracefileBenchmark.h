#pragma once

#include "benchmark.h"
#include "jdbc/connector.h"

class TracefileBenchmark : public Benchmark
{
public:
    TracefileBenchmark(MySQLConnector& conn, std::string tracefile);
    ~TracefileBenchmark() = default;
    bool create_database() { return true; }
    bool load_database() { return true; }
    bool step();
    bool is_end();
    bool cleanup();
    bool execute_statement(std::string statement);

public:
    std::string tracefile;
    std::vector<std::string> statements;
    unsigned int current_statement = 0;
};