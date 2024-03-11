#pragma once

#include "common.h"
#include "jdbc/connector.h"

class Benchmark
{
public:
    MySQLConnector& connector;
    std::string name;
    Benchmark(MySQLConnector& conn) : connector(conn) {};
    virtual ~Benchmark() {};
    virtual bool create_database() = 0;
    virtual bool load_database() = 0;
    virtual bool step() = 0;
    virtual bool is_end();
    virtual bool run();
    virtual bool cleanup() = 0;

    unsigned int records_executed = 0;
    unsigned int millis_elapsed = 0; // in milliseconds
};