#include "cpu_metrics.h"
#include "gpu_metrics.h"
#include "database_manager.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    CPUMetrics cpuMetrics;
    GPUMetrics gpuMetrics;
    
    // Initialize DatabaseManager with batch size of 10
    DatabaseManager dbManager("metrics.db", 10);
    if (!dbManager.initialize()) {
        std::cerr << "Failed to initialize database. Exiting." << std::endl;
        return 1;
    }

    if (!gpuMetrics.initSuccess) {
        std::cerr << "Failed to initialize NVML or find GPU." << std::endl;
    } else {
        std::cout << "Monitoring GPU: " << gpuMetrics.getGPUName() << std::endl;
    }

    cpuMetrics.getCPUUsage(); // Call once to initialize prev times
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "Starting Telemetry Logger... (Batching every 10s to DB. CTRL+C to quit)\n";
    
    while (true) {
        double cpuUse = cpuMetrics.getCPUUsage();
        double ramUse = cpuMetrics.getRAMUsage();
        unsigned int gpuTemp = gpuMetrics.getTemperature();
        unsigned int gpuUtil = gpuMetrics.getUtilization();

        printf("[Metrics] CPU: %.2f%% | RAM: %.2f%% | GPU Temp: %uC | GPU Util: %u%%\n", 
               cpuUse, ramUse, gpuTemp, gpuUtil);

        // Add to database buffer
        dbManager.insertRecord(cpuUse, ramUse, gpuTemp, gpuUtil);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}