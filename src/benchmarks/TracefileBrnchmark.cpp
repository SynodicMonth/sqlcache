#include "TracefileBenchmark.h"

TracefileBenchmark::TracefileBenchmark(MySQLConnector& conn, std::string tracefile)
    : Benchmark(conn) {
    this->tracefile = tracefile;
    name = "TracefileBenchmark";
    std::ifstream file(tracefile);
    std::string line;
    std::stringstream sql_stream;
    while (std::getline(file, line)) {
        sql_stream << line << std::endl;
    }
    std::string statement;
    while (std::getline(sql_stream, statement, ';')) {
        if (statement.find_first_not_of(' ') != std::string::npos 
            && statement.find_first_not_of('\n') != std::string::npos) {
            statements.push_back(statement);
        }
    }
    std::cout << "TracefileBenchmark: " << statements.size() << " statements" << std::endl;
}

bool TracefileBenchmark::step() {
    if (current_statement < statements.size()) {
        execute_statement(statements[current_statement]);
        current_statement++;
        return true;
    }
    return false;
}

bool TracefileBenchmark::is_end() {
    return current_statement >= statements.size();
}

bool TracefileBenchmark::cleanup() {
    return true;
}

bool TracefileBenchmark::execute_statement(std::string statement) {
    std::cout << "TracefileBenchmark: execute_statement: " << statement << std::endl;
    // if the statement is a select statement, we need to fetch the result
    // TODO: match the statement with a regex
    if (statement.find("SELECT") != std::string::npos
        || statement.find("select") != std::string::npos
        || statement.find("Select") != std::string::npos
        || statement.find("SHOW") != std::string::npos
        || statement.find("show") != std::string::npos
        || statement.find("Show") != std::string::npos) {
        try {
            std::unique_ptr<sql::ResultSet> result = connector.execute_query(statement);
            // print out the whole result
            while (result->next()) {
                std::cout << "TracefileBenchmark: resultset: " << std::endl;
                std::cout << result->getString(1) << std::endl;
            }
        } catch (sql::SQLException &e) {
            std::cout << "TracefileBenchmark: execute_statement: " << e.what() << std::endl;
            return false;
        }
    } else {
        try {
            connector.execute_update(statement);
        } catch (sql::SQLException &e) {
            std::cout << "TracefileBenchmark: execute_statement: " << e.what() << std::endl;
            return false;
        }
    }
    return true;
}
