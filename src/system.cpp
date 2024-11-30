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

#include "system.h"


const double WINDOW_DELAY = 0.7;
const std::vector<std::string> WINDOW_EXCLUDE_LIST = {
    "GDI+",
    "DDE Server Window",
    "Default IME",
    "MSCTFIME UI",
    ".NET-BroadcastEventWindow",
    "DesktopWindowXamlSource",
    "HardwareMonitorWindow",
    "WISPTIS",
    "SystemResourceNotifyWindow",
    "DesktopWindow",
    "Battery Meter",
    "BluetoothNotificationAreaIconWindowClass",
    "CiceroUIWndFrame",
    "DesktopInfo",
    "MediaContextNotificationWindow",
    "TclNotifier",
    "Task Switching",
    "Task Host Window",
    "Shell Handwriting Canvas",
    "System tray overflow window.",
    "SecurityHealthSystray",
    "Win HCP",
    "Windows Input Experience",
    "OfficePowerManagerWindow",
    "PopupHost",
    "Progress",
    "RealtekAudioAdminBackgroundProcessClass",
    "CFD File Open Message Window",
    "Core Sync",
    "Desktop Info",
    "Graphics Command Center",
    "MenuWindow",
    "Microsoft OneNote - Windows taskbar",
    "MS_WebcheckMonitor",
    "Network Flyout",
    "Per Monitor Aware Window",
    "Program Manager",
    "Rtc Video PnP Listener",
    "TtkMonitorWindow",
    "Windows Push Notifications Platform",
    "DWM Notification Window"
};

bool Window::operator==(const Window& other) const {
    return (title == other.title) &&
           (class_name == other.class_name) &&
           (hwnd == other.hwnd);
}

bool is_valid(const char* title) {
    std::string caption(title);

    for (const auto& word : WINDOW_EXCLUDE_LIST) {
        if (caption.find(word) != std::string::npos) {
            return false;
        }
    }

    return true;
}

std::string get_cpu_name() {
    HKEY hkey;
    char cpu_name[256];
    DWORD buffer_size = sizeof(cpu_name);

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                      0, KEY_READ, &hkey) == ERROR_SUCCESS) {
        RegQueryValueExA(
            hkey,
            "ProcessorNameString",
            NULL, NULL,
            (LPBYTE)cpu_name,
            &buffer_size);

        RegCloseKey(hkey);
    }
    return std::string(cpu_name);
}

std::string get_gpu_name() {
    DISPLAY_DEVICEA dd;
    dd.cb = sizeof(dd);
    std::string name = "Unknown GPU";

    if (EnumDisplayDevicesA(NULL, 0, &dd, 0)) {
        name = dd.DeviceString;
    }
    return name;
}

std::string get_os_info() {
    /*
    Retrieve OS information, such as Windows build information.
    */

    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    // Get architecture
    std::string arch = (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) ? "64-bit" : "32-bit";

    OSVERSIONINFOEXA osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    GetVersionExA((LPOSVERSIONINFOA)&osvi);

    std::ostringstream os_info;
    os_info << "Windows " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion
           << " (Build " << osvi.dwBuildNumber << "), " << arch;
    return os_info.str();
}

std::string get_available_ram() {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    std::ostringstream ram_info;
    ram_info << (statex.ullAvailPhys / (1024 * 1024)) << " MB available";
    return ram_info.str();
}

void press(BYTE key) {
    /*
    Press a key.
    */

    keybd_event(key, 0, 0, 0);
}

void release(BYTE key) {
    /*
    Release a held key.
    */

    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}

inline void push(BYTE key) {
    /*
    Shortcut to push and release keys.
    */

    press(key);
    release(key);
}

void toggle_internet() {
    /*
    Toggle internet by the following keypresses:
    
    1. Press Windows-A
    2. Press Space
    3. Press Windows-A
    4. Press Escape to close the window
    */

    press(0x5B);
    press(0x41);
    release(0x41);
    release(0x5b);

    std::this_thread::sleep_for(std::chrono::duration<double>(WINDOW_DELAY));

    push(0x20); // Space
    push(0x1B); // Escape
}

BOOL CALLBACK enum_windows(HWND hwnd, LPARAM lParam) {
    std::vector<Window>* windows = reinterpret_cast<std::vector<Window>*>(lParam);

    char title[256];
    char class_name[256];

    GetWindowText(hwnd, title, sizeof(title));
    GetClassNameA(hwnd, class_name, sizeof(class_name));

    if (title[0]) {
        if (is_valid(title)) {
            windows->push_back({hwnd, title, class_name});
        }
    }

    return TRUE;
}

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    DWORD code = ExceptionInfo->ExceptionRecord->ExceptionCode;
    PVOID address = ExceptionInfo->ExceptionRecord->ExceptionAddress;

    std::cerr << "Exception Code: " << code << std::endl;
    std::cerr << "Exception Address: " << address << std::endl;

    CONTEXT* context = ExceptionInfo->ContextRecord;
    std::cerr << "RAX: " << context->Rax << std::endl;
    std::cerr << "RBX: " << context->Rbx << std::endl;
    std::cerr << "RCX: " << context->Rcx << std::endl;
    std::cerr << "RDX: " << context->Rdx << std::endl;

    return EXCEPTION_EXECUTE_HANDLER;
}
