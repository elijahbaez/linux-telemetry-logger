# ==========================================
# STAGE 1: Build Environment
# ==========================================
FROM ubuntu:22.04 AS builder

# Prevent tzdata from asking for geographic location during install
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies and CUDA toolkit for NVML headers
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libsqlite3-dev \
    nvidia-cuda-toolkit

WORKDIR /src
COPY . .

# Compile the application
RUN mkdir build && cd build && cmake .. && make

# ==========================================
# STAGE 2: Lightweight Runtime
# ==========================================
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install only the runtime dependency (SQLite)
# Note: NVML drivers are automatically injected by the NVIDIA Container Toolkit at runtime!
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Create a data directory for the SQLite database volume mount
RUN mkdir data

# Copy only the compiled binary from Stage 1
COPY --from=builder /src/build/telemetry_logger .

# Execute the logger
CMD ["./telemetry_logger"]# ==========================================
# STAGE 1: Build Environment
# ==========================================
FROM ubuntu:22.04 AS builder

# Prevent tzdata from asking for geographic location during install
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies and CUDA toolkit for NVML headers
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libsqlite3-dev \
    nvidia-cuda-toolkit

WORKDIR /src
COPY . .

# Compile the application
RUN mkdir build && cd build && cmake .. && make

# ==========================================
# STAGE 2: Lightweight Runtime
# ==========================================
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install only the runtime dependency (SQLite)
# Note: NVML drivers are automatically injected by the NVIDIA Container Toolkit at runtime!
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Create a data directory for the SQLite database volume mount
RUN mkdir data

# Copy only the compiled binary from Stage 1
COPY --from=builder /src/build/telemetry_logger .

# Execute the logger
CMD ["./telemetry_logger"]
