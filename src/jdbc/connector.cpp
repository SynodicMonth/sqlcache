#include "connector.h"

MySQLConnector::MySQLConnector() {
    driver = sql::mysql::get_mysql_driver_instance();
}

MySQLConnector::~MySQLConnector() {
    conn->close();
}

bool MySQLConnector::initialize(std::string url, std::string username, std::string password, std::string schema) {
    conn = std::unique_ptr<sql::Connection>(driver->connect(url, username, password));
    conn->setSchema(schema);
    return true;
}

bool MySQLConnector::execute_update(std::string statement) {
    std::unique_ptr<sql::Statement> stmt(conn->createStatement());
    stmt->execute(statement);
    return true;
}

std::unique_ptr<sql::ResultSet> MySQLConnector::execute_query(std::string statement) {
    std::unique_ptr<sql::Statement> stmt(conn->createStatement());
    return std::unique_ptr<sql::ResultSet>(stmt->executeQuery(statement));
}
