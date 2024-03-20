#include "YCSBBenchmark.h"
#include <thread>

YCSBBenchmark::YCSBBenchmark(MySQLConnector& conn)
    : Benchmark(conn) { 
        name = "YCSB"; 
        generator = std::default_random_engine(std::time(0));
        zipfian_distrib = zipfian_int_distribution<int>(0, max_records - 1, 0.99);
        rng = std::mt19937(rd());
        next_key = max_records;
        stmt = std::unique_ptr<sql::Statement>(connector.conn->createStatement());
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
    stmt->execute("SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;");
    stmt->execute("SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;");
    stmt->execute("DROP TABLE IF EXISTS usertable;");
    stmt->execute("CREATE TABLE usertable (ycsb_key int PRIMARY KEY, field1 varchar(100), field2 varchar(100), field3 varchar(100), field4 varchar(100), field5 varchar(100), field6 varchar(100), field7 varchar(100), field8 varchar(100), field9 varchar(100), field10 varchar(100));");
    stmt->execute("SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;");
    stmt->execute("SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;");
    insert_stmt = std::unique_ptr<sql::PreparedStatement>(
            connector.conn->prepareStatement("INSERT INTO usertable (ycsb_key, field1, field2, field3, field4, field5, field6, field7, field8, field9, field10) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)")
        );
    std::cout << "YCSBBenchmark: create_database: done" << std::endl;
    return true;
}

bool YCSBBenchmark::load_database() {
    /*
    INSERT INTO usertable (ycsb_key, field1, field2, field3, field4, field5, field6, field7, field8, field9, field10) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    */
    connector.conn->setAutoCommit(false);
    for (int i = 0; i < max_records; i++) {
        insert_stmt->setInt(1, i);
        for (int j = 0; j < max_fields; j++) {
            insert_stmt->setString(j + 2, randomFastString(rng, max_field_size));
        }
        insert_stmt->executeUpdate();

        if (i % 1000 == 0) {
            connector.conn->commit();
        }
    }
    connector.conn->commit();
    connector.conn->setAutoCommit(true);
    return true;
}

bool YCSBBenchmark::step() {
    // excute different queries based on the proportion
    if (rng() % 100 < query_proportion * 100) {
        std::vector<std::string> results;
        read_record(zipfian_distrib(generator), results);
    } else {
        std::vector<std::string> values;
        for (int i = 0; i < max_fields; i++) {
            values.push_back(randomFastString(rng, max_field_size));
        }
        insert_record(next_key++, values);
    }
    current_query++;
    return true;
}

bool YCSBBenchmark::read_record(int key, std::vector<std::string>& results) {
    std::string sql = generate_read_sql(key);
    // std::cout << "YCSBBenchmark: read_record: " << key << std::endl;
    if (cache_enabled) {
        // if cache is enabled, try to read from cache first
        std::string value = get_(key);
        if (value != "") {
            results.push_back(value);
            return true;
        }
    }
    std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(sql));
    // std::cout << "YCSBBenchmark: resultset: " << std::endl;
    for (int i = 1; i <= max_fields; i++) {
        // std::cout << result->getString(i) << std::endl;
        results.push_back(result->getString(i));
    }
    // make sure results are not optimized away
    if (results.size() == 0) {
        std::cout << "YCSBBenchmark: read_record: " << key << " failed" << std::endl;
        return false;
    }
    if (cache_enabled) {
        set_(key, results[0]); // just cache the first field for now
    }
    return true;
}

bool YCSBBenchmark::insert_record(int key, std::vector<std::string>& values) {
    insert_stmt->setInt(1, key);
    for (int i = 0; i < max_fields; i++) {
        insert_stmt->setString(i + 2, values[i]);
    }
    insert_stmt->executeUpdate();
    if (cache_enabled) {
        invalidateAll();
    }
    return true;
}


bool YCSBBenchmark::is_end() {
    return current_query >= max_query;
}

bool YCSBBenchmark::cleanup() {
    return true;
}

std::string YCSBBenchmark::generate_read_sql(int key) {
    std::string sql = "SELECT * FROM usertable WHERE YCSB_KEY = " + std::to_string(key);
    return sql;
}

std::string YCSBBenchmark::generate_insert_sql(int key, std::vector<std::string>& values) {
    std::string sql = "INSERT INTO usertable (ycsb_key, field1, field2, field3, field4, field5, field6, field7, field8, field9, field10) VALUES (" + std::to_string(key);
    for (int i = 0; i < max_fields; i++) {
        sql += ", '" + values[i] + "'";
    }
    sql += ")";
    return sql;
}

void YCSBBenchmark::enable_cache() {
    if (cache_enabled) {
        std::cout << "YCSBBenchmark: cache is already enabled" << std::endl;
    }
    cache_enabled = true;
    initializeCache();
    addpool_("ycsb");
    std::cout << "YCSBBenchmark: cache enabled" << std::endl;
}