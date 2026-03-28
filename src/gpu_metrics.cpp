#include "gpu_metrics.h"
#include <iostream>

GPUMetrics::GPUMetrics() : initSuccess(false) {
    if (nvmlInit() == NVML_SUCCESS) {
        if (nvmlDeviceGetHandleByIndex(0, &device) == NVML_SUCCESS) {
            initSuccess = true;
        }
    }
}

GPUMetrics::~GPUMetrics() {
    if (initSuccess) {
        nvmlShutdown();
    }
}

unsigned int GPUMetrics::getTemperature() {
    unsigned int temp = 0;
    if (initSuccess) nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
    return temp;
}

unsigned int GPUMetrics::getUtilization() {
    nvmlUtilization_t util;
    if (initSuccess) {
        nvmlDeviceGetUtilizationRates(device, &util);
        return util.gpu;
    }
    return 0;
}

unsigned int GPUMetrics::getMemoryUtilization() {
    nvmlUtilization_t util;
    if (initSuccess) {
        nvmlDeviceGetUtilizationRates(device, &util);
        return util.memory;
    }
    return 0;
}

std::string GPUMetrics::getGPUName() {
    char name[NVML_DEVICE_NAME_BUFFER_SIZE];
    if (initSuccess && nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE) == NVML_SUCCESS) {
        return std::string(name);
    }
    return "Unknown GPU";
}