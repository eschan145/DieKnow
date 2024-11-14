/*
COPYRIGHT (C) 2024 ETHAN CHAN

ALL RIGHTS RESERVED. UNAUTHORIZED COPYING, MODIFICATION, DISTRIBUTION, OR USE
OF THIS SOFTWARE WITHOUT PRIOR PERMISSION IS STRICTLY PROHIBITED.

THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM,
OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

PROJECT NAME: DieKnow
FILENAME: src/system.cpp
DESCRIPTION: System information for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-13
VERSION: 1.0.1
*/

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
