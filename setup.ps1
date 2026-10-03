$ErrorActionPreference = "Stop"
if (-not (Test-Path "external\FreeRTOS-Kernel\CMakeLists.txt")) {
    New-Item -ItemType Directory -Force external | Out-Null
    git clone --depth 1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git external/FreeRTOS-Kernel
}
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
