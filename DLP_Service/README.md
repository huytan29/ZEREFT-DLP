# Lancs DLP

Lancs DLP agent project 

## Tham khảo

### Windows File System Filter Driver

- (Giới thiệu)[https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/filter-manager-concepts]

- (Code mẫu)[https://learn.microsoft.com/en-us/windows-hardware/drivers/samples/file-system-driver-samples]

- (Code mẫu block device theo type)[https://www.codeproject.com/Articles/5341729/Mini-filter-to-block-certain-media-types]

### Windows Networking Driver
- (Giới thiệu)[https://learn.microsoft.com/en-us/windows-hardware/drivers/network/]
- (Code mẫu)[https://learn.microsoft.com/en-us/windows-hardware/drivers/samples/networking-driver-samples]

#### Hooking với MS Detours
- https://github.com/microsoft/Detours
- https://github.com/Microsoft/Detours/wiki/Using-Detours

### How to build project with Cmake
Step 1: Download Visual Studio and Windows SDK

Step 2: Download WFPExplorer (https://github.com/zodiacon/WFPExplorer/releases/tag/v0.5.4) for delete rule of windows filtering platform (rule name: LancsDLP's WFPFirewall Sublayer)

Step 3: Build source using Cmake file
1. cd build
2. cmake -G "Visual Studio 17" -A x64 ..
3. cmake --build .
4. run DLP.exe in folder "build/Debug"

