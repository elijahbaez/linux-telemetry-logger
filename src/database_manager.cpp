#include "database_manager.h"
#include <iostream>
#include <chrono>
DatabaseManager::DatabaseManager(const std::string& path, size_t batch) : db_path(path), batch_size(batch), db(nullptr) { buffer.reserve(batch_size); }
DatabaseManager::~DatabaseManager() { if (!buffer.empty()) flushBatch(); if (db) sqlite3_close(db); }
bool DatabaseManager::initialize() {
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) return false;
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS telemetry (timestamp INTEGER, cpu REAL, ram REAL, gpu_temp INTEGER, gpu_util INTEGER, vram_usage INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS sysinfo (id INTEGER PRIMARY KEY, cpu_name TEXT, gpu_name TEXT, total_ram TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, nullptr);
    return true;
}
bool DatabaseManager::insertSysInfo(const std::string& cpu, const std::string& gpu, const std::string& ram) {
    sqlite3_exec(db, "DELETE FROM sysinfo;", nullptr, nullptr, nullptr);
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "INSERT INTO sysinfo (id, cpu_name, gpu_name, total_ram) VALUES (1, ?, ?, ?);", -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, cpu.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, gpu.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, ram.c_str(), -1, SQLITE_TRANSIENT);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt); return success;
}
void DatabaseManager::insertRecord(double cpu, double ram, unsigned int temp, unsigned int util, unsigned int vram) {
    long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    buffer.push_back({timestamp, cpu, ram, temp, util, vram});
    if (buffer.size() >= batch_size) flushBatch();
}
void DatabaseManager::flushBatch() {
    if (buffer.empty()) return;
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "INSERT INTO telemetry (timestamp, cpu, ram, gpu_temp, gpu_util, vram_usage) VALUES (?, ?, ?, ?, ?, ?);", -1, &stmt, nullptr);
    for (const auto& record : buffer) {
        sqlite3_bind_int64(stmt, 1, record.timestamp); sqlite3_bind_double(stmt, 2, record.cpu_usage); sqlite3_bind_double(stmt, 3, record.ram_usage);
        sqlite3_bind_int(stmt, 4, record.gpu_temp); sqlite3_bind_int(stmt, 5, record.gpu_util); sqlite3_bind_int(stmt, 6, record.vram_usage);
        sqlite3_step(stmt); sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt); sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    buffer.clear(); std::cout << "[Database] Real-time metric logged." << std::endl;
}
