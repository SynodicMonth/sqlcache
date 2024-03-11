#include "YCSBBenchmark.h"

YCSBBenchmark::YCSBBenchmark(MySQLConnector& conn)
    : Benchmark(conn) { 
        name = "YCSB"; 
        read_stmt = std::unique_ptr<sql::PreparedStatement>(
            connector.conn->prepareStatement("SELECT * FROM usertable WHERE YCSB_KEY = ?")
        );
        generator = std::default_random_engine(std::time(0));
        distribution = zipfian_int_distribution<int>(0, max_records - 1, 0.99);
    }

bool YCSBBenchmark::create_database() {
    /*
    SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
    SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
    DROP TABLE IF EXISTS usertable;
    CREATE TABLE usertable (
        ycsb_key int PRIMARY KEY,
        field1   varchar(100),
        field2   varchar(100),
        field3   varchar(100),
        field4   varchar(100),
        field5   varchar(100),
        field6   varchar(100),
        field7   varchar(100),
        field8   varchar(100),
        field9   varchar(100),
        field10  varchar(100)
    );
    SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
    SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
    */
    std::unique_ptr<sql::Statement> stmt(connector.conn->createStatement());
    stmt->execute("SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;");
    stmt->execute("SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;");
    stmt->execute("DROP TABLE IF EXISTS usertable;");
    stmt->execute("CREATE TABLE usertable (ycsb_key int PRIMARY KEY, field1 varchar(100), field2 varchar(100), field3 varchar(100), field4 varchar(100), field5 varchar(100), field6 varchar(100), field7 varchar(100), field8 varchar(100), field9 varchar(100), field10 varchar(100));");
    stmt->execute("SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;");
    stmt->execute("SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;");
    return true;
}

bool YCSBBenchmark::load_database() {
    /*
    INSERT INTO usertable (ycsb_key, field1, field2, field3, field4, field5, field6, field7, field8, field9, field10) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    */
    connector.conn->setAutoCommit(false);
    std::unique_ptr<sql::PreparedStatement> stmt(connector.conn->prepareStatement("INSERT INTO usertable (ycsb_key, field1, field2, field3, field4, field5, field6, field7, field8, field9, field10) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"));
    
    std::random_device rd;
    std::mt19937 rng(rd());

    for (int i = 0; i < max_records; i++) {
        stmt->setInt(1, i);
        for (int j = 0; j < max_fields; j++) {
            stmt->setString(j + 2, randomFastString(rng, max_field_size));
        }
        stmt->executeUpdate();

        if (i % 1000 == 0) {
            connector.conn->commit();
        }
    }
    connector.conn->commit();
    connector.conn->setAutoCommit(true);
    return true;
}

bool YCSBBenchmark::step() {
    read_record(distribution(generator));
    current_query++;
    return true;
}

bool YCSBBenchmark::read_record(int key) {
    read_stmt->setInt(1, key);
    // std::cout << "YCSBBenchmark: read_record: " << key << std::endl;
    std::unique_ptr<sql::ResultSet> result(read_stmt->executeQuery());
    // while (result->next()) {
    //     std::cout << "YCSBBenchmark: resultset: " << std::endl;
    //     for (int i = 1; i <= max_fields; i++) {
    //         std::cout << result->getString(i) << std::endl;
    //     }
    // }
    return true;
}

bool YCSBBenchmark::is_end() {
    return current_query >= max_query;
}

bool YCSBBenchmark::cleanup() {
    return true;
}
