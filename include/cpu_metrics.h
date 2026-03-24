#ifndef CPU_METRICS_H
#define CPU_METRICS_H

class CPUMetrics {
public:
    CPUMetrics();
    double getCPUUsage();
    double getRAMUsage(); // Returns percentage of RAM used

private:
    unsigned long long prevIdleTime;
    unsigned long long prevTotalTime;
};

#endif