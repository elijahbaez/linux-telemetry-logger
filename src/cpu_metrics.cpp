#include "cpu_metrics.h"
#include <fstream>
#include <sstream>
#include <vector>
CPUMetrics::CPUMetrics() : prevIdleTime(0), prevTotalTime(0) {}
double CPUMetrics::getCPUUsage() {
    std::ifstream file("/proc/stat");
    std::string line; std::getline(file, line); file.close();
    std::istringstream ss(line);
    std::string cpuLabel; ss >> cpuLabel;
    std::vector<unsigned long long> times;
    unsigned long long time;
    while (ss >> time) times.push_back(time);
    if (times.size() < 4) return 0.0;
    unsigned long long idleTime = times[3] + times[4];
    unsigned long long totalTime = 0;
    for (auto t : times) totalTime += t;
    double deltaIdle = idleTime - prevIdleTime;
    double deltaTotal = totalTime - prevTotalTime;
    prevIdleTime = idleTime; prevTotalTime = totalTime;
    if (deltaTotal == 0) return 0.0;
    return (1.0 - (deltaIdle / deltaTotal)) * 100.0;
}
double CPUMetrics::getRAMUsage() {
    std::ifstream file("/proc/meminfo");
    std::string line; long totalMem = 0, availableMem = 0;
    while (std::getline(file, line)) {
        if (line.find("MemTotal:") == 0) sscanf(line.c_str(), "MemTotal: %ld kB", &totalMem);
        else if (line.find("MemAvailable:") == 0) { sscanf(line.c_str(), "MemAvailable: %ld kB", &availableMem); break; }
    }
    file.close();
    if (totalMem == 0) return 0.0;
    return (1.0 - ((double)availableMem / totalMem)) * 100.0;
}
std::string CPUMetrics::getCPUName() {
    std::ifstream file("/proc/cpuinfo"); std::string line;
    while (std::getline(file, line)) {
        if (line.find("model name") == 0) {
            size_t colon = line.find(":");
            if (colon != std::string::npos) {
                size_t start = line.find_first_not_of(" \t", colon + 1);
                if (start != std::string::npos) return line.substr(start);
            }
        }
    }
    return "Unknown CPU";
}
std::string CPUMetrics::getTotalRAM() {
    std::ifstream file("/proc/meminfo"); std::string line; long totalMem = 0;
    while (std::getline(file, line)) {
        if (line.find("MemTotal:") == 0) { sscanf(line.c_str(), "MemTotal: %ld kB", &totalMem); break; }
    }
    char buf[32]; snprintf(buf, sizeof(buf), "%.1f GB", totalMem / (1024.0 * 1024.0));
    return std::string(buf);
}
