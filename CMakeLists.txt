cmake_minimum_required(VERSION 3.10)
project(LinuxTelemetryLogger)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# Added src/database_manager.cpp
add_executable(telemetry_logger src/main.cpp src/cpu_metrics.cpp src/gpu_metrics.cpp src/database_manager.cpp)

# Added sqlite3 to the linked libraries
target_link_libraries(telemetry_logger nvidia-ml sqlite3)