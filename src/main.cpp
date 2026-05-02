#include "cpu_metrics.h"
#include "gpu_metrics.h"
#include "database_manager.h"
#include <iostream>
#include <thread>
#include <chrono>
int main() {
    CPUMetrics cpuMetrics; GPUMetrics gpuMetrics;
    DatabaseManager dbManager("data/metrics.db", 1);
    if (!dbManager.initialize()) return 1;
    
    std::cout << "Monitoring GPU: " << gpuMetrics.getGPUName() << std::endl;
    
    // Concatenate GPU Name and Total VRAM here!
    std::string gpuInfo = gpuMetrics.getGPUName() + " (" + gpuMetrics.getTotalVRAM() + " VRAM)";
    dbManager.insertSysInfo(cpuMetrics.getCPUName(), gpuInfo, cpuMetrics.getTotalRAM());
    
    cpuMetrics.getCPUUsage(); 
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    while (true) {
        double cpuUse = cpuMetrics.getCPUUsage(); double ramUse = cpuMetrics.getRAMUsage();
        unsigned int gpuTemp = gpuMetrics.getTemperature(); unsigned int gpuUtil = gpuMetrics.getUtilization();
        unsigned int vramUse = gpuMetrics.getVRAMUsage();

        printf("[Metrics] CPU: %.2f%% | RAM: %.2f%% | GPU Temp: %uC | GPU Util: %u%% | VRAM: %u%%\n", cpuUse, ramUse, gpuTemp, gpuUtil, vramUse);
        dbManager.insertRecord(cpuUse, ramUse, gpuTemp, gpuUtil, vramUse);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
