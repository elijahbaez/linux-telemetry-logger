#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H
#include <string>
#include <vector>
#include <sqlite3.h>
struct TelemetryRecord {
    long long timestamp; double cpu_usage; double ram_usage;
    unsigned int gpu_temp; unsigned int gpu_util; unsigned int vram_usage;
};
class DatabaseManager {
public:
    DatabaseManager(const std::string& db_path, size_t batch_size = 1);
    ~DatabaseManager();
    bool initialize();
    bool insertSysInfo(const std::string& cpu, const std::string& gpu, const std::string& ram);
    void insertRecord(double cpu, double ram, unsigned int temp, unsigned int util, unsigned int vram);
private:
    void flushBatch();
    std::string db_path; size_t batch_size; sqlite3* db; std::vector<TelemetryRecord> buffer;
};
#endif
