#ifndef CPU_METRICS_H
#define CPU_METRICS_H
#include <string>
class CPUMetrics {
public:
    CPUMetrics();
    double getCPUUsage();
    double getRAMUsage();
    std::string getCPUName();
    std::string getTotalRAM();
private:
    unsigned long long prevIdleTime;
    unsigned long long prevTotalTime;
};
#endif
