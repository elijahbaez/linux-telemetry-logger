#include "database_manager.h"
#include <iostream>
#include <chrono>

DatabaseManager::DatabaseManager(const std::string& path, size_t batch) 
    : db_path(path), batch_size(batch), db(nullptr) {
    buffer.reserve(batch_size);
}

DatabaseManager::~DatabaseManager() {
    if (!buffer.empty()) {
        flushBatch(); // Flush any remaining records before closing
    }
    if (db) {
        sqlite3_close(db);
    }
}

bool DatabaseManager::initialize() {
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open DB: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    const char* create_table_sql = 
        "CREATE TABLE IF NOT EXISTS telemetry ("
        "timestamp INTEGER, "
        "cpu REAL, "
        "ram REAL, "
        "gpu_temp INTEGER, "
        "gpu_util INTEGER);";

    char* err_msg = nullptr;
    if (sqlite3_exec(db, create_table_sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        std::cerr << "SQL Error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

void DatabaseManager::insertRecord(double cpu, double ram, unsigned int temp, unsigned int util) {
    auto now = std::chrono::system_clock::now();
    long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    buffer.push_back({timestamp, cpu, ram, temp, util});

    if (buffer.size() >= batch_size) {
        flushBatch();
    }
}

void DatabaseManager::flushBatch() {
    if (buffer.empty()) return;

    // Use a transaction for fast bulk inserts
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    sqlite3_stmt* stmt;
    const char* insert_sql = "INSERT INTO telemetry (timestamp, cpu, ram, gpu_temp, gpu_util) VALUES (?, ?, ?, ?, ?);";
    
    sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr);

    for (const auto& record : buffer) {
        sqlite3_bind_int64(stmt, 1, record.timestamp);
        sqlite3_bind_double(stmt, 2, record.cpu_usage);
        sqlite3_bind_double(stmt, 3, record.ram_usage);
        sqlite3_bind_int(stmt, 4, record.gpu_temp);
        sqlite3_bind_int(stmt, 5, record.gpu_util);

        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    
    buffer.clear();
    std::cout << "[Database] Flushed batch to SQLite successfully." << std::endl;
}