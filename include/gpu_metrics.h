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
    unsigned int getMemoryUtilization();
    std::string getGPUName();

private:
    nvmlDevice_t device;
};

#endif