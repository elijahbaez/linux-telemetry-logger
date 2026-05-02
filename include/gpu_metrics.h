#ifndef GPU_METRICS_H
#define GPU_METRICS_H
#include <nvml.h>
#include <string>
class GPUMetrics {
public:
    GPUMetrics();
    ~GPUMetrics();
    bool initSuccess;
    unsigned int getTemperature();
    unsigned int getUtilization();
    unsigned int getVRAMUsage();
    std::string getGPUName();
    std::string getTotalVRAM();
private:
    nvmlDevice_t device;
};
#endif
