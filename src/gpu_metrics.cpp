#include "gpu_metrics.h"
GPUMetrics::GPUMetrics() : initSuccess(false) {
    if (nvmlInit() == NVML_SUCCESS) {
        if (nvmlDeviceGetHandleByIndex(0, &device) == NVML_SUCCESS) initSuccess = true;
    }
}
GPUMetrics::~GPUMetrics() { if (initSuccess) nvmlShutdown(); }
unsigned int GPUMetrics::getTemperature() {
    unsigned int temp = 0;
    if (initSuccess) nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
    return temp;
}
unsigned int GPUMetrics::getUtilization() {
    nvmlUtilization_t util;
    if (initSuccess) { nvmlDeviceGetUtilizationRates(device, &util); return util.gpu; }
    return 0;
}
unsigned int GPUMetrics::getVRAMUsage() {
    nvmlMemory_t mem;
    if (initSuccess && nvmlDeviceGetMemoryInfo(device, &mem) == NVML_SUCCESS) {
        return (unsigned int)((mem.used * 100.0) / mem.total);
    }
    return 0;
}
std::string GPUMetrics::getGPUName() {
    char name[NVML_DEVICE_NAME_BUFFER_SIZE];
    if (initSuccess && nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE) == NVML_SUCCESS) return std::string(name);
    return "Unknown GPU";
}
std::string GPUMetrics::getTotalVRAM() {
    nvmlMemory_t mem;
    if (initSuccess && nvmlDeviceGetMemoryInfo(device, &mem) == NVML_SUCCESS) {
        char buf[32]; snprintf(buf, sizeof(buf), "%.1f GB", mem.total / (1024.0 * 1024.0 * 1024.0));
        return std::string(buf);
    }
    return "Unknown";
}
