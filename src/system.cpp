#include <windows.h>
#include <string>
#include <sstream>

std::string get_cpu_name() {
    HKEY hKey;
    char cpuName[256];
    DWORD bufSize = sizeof(cpuName);

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)cpuName, &bufSize);
        RegCloseKey(hKey);
    }
    return std::string(cpuName);
}

std::string get_gpu_name() {
    DISPLAY_DEVICEA dd;
    dd.cb = sizeof(dd);
    std::string gpuName = "Unknown GPU";

    if (EnumDisplayDevicesA(NULL, 0, &dd, 0)) {
        gpuName = dd.DeviceString;
    }
    return gpuName;
}

std::string get_os_info() {
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    std::string arch = (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) ? "64-bit" : "32-bit";

    OSVERSIONINFOEXA osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    GetVersionExA((LPOSVERSIONINFOA)&osvi);

    std::ostringstream osInfo;
    osInfo << "Windows " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion
           << " (Build " << osvi.dwBuildNumber << "), " << arch;
    return osInfo.str();
}

std::string get_available_ram() {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    std::ostringstream ramInfo;
    ramInfo << (statex.ullAvailPhys / (1024 * 1024)) << " MB available";
    return ramInfo.str();
}
