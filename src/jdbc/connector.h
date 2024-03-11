#pragma once

#include "common.h"
#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class MySQLConnector {
public:
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> conn;

    MySQLConnector();
    ~MySQLConnector();
    bool initialize(std::string url, std::string username, std::string password, std::string schema);
    bool execute_update(std::string statement);
    std::unique_ptr<sql::ResultSet> execute_query(std::string statement);
};