#include "cpu_metrics.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

CPUMetrics::CPUMetrics() : prevIdleTime(0), prevTotalTime(0) {}

double CPUMetrics::getCPUUsage() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    file.close();

    std::istringstream ss(line);
    std::string cpuLabel;
    ss >> cpuLabel;

    std::vector<unsigned long long> times;
    unsigned long long time;
    while (ss >> time) {
        times.push_back(time);
    }

    if (times.size() < 4) return 0.0;

    unsigned long long idleTime = times[3] + times[4]; // idle + iowait
    unsigned long long totalTime = 0;
    for (auto t : times) {
        totalTime += t;
    }

    double deltaIdle = idleTime - prevIdleTime;
    double deltaTotal = totalTime - prevTotalTime;

    prevIdleTime = idleTime;
    prevTotalTime = totalTime;

    if (deltaTotal == 0) return 0.0;
    return (1.0 - (deltaIdle / deltaTotal)) * 100.0;
}

double CPUMetrics::getRAMUsage() {
    std::ifstream file("/proc/meminfo");
    std::string line;
    long totalMem = 0, availableMem = 0;

    while (std::getline(file, line)) {
        if (line.find("MemTotal:") == 0) {
            sscanf(line.c_str(), "MemTotal: %ld kB", &totalMem);
        } else if (line.find("MemAvailable:") == 0) {
            sscanf(line.c_str(), "MemAvailable: %ld kB", &availableMem);
            break; 
        }
    }
    file.close();

    if (totalMem == 0) return 0.0;
    return (1.0 - ((double)availableMem / totalMem)) * 100.0;
}