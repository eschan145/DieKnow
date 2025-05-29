/*
COPYRIGHT (C) 2025 ETHAN CHAN

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
FILENAME: src/api.h
DESCRIPTION: API functions for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-13
VERSION: 1.0.1

Compile with g++ -shared -o api.dll api.cpp -Ofast -fPIC -shared
*/

#ifndef API_H
#define API_H

// Expose functions marked with DK_API for DLLs
#define DK_API __declspec(dllexport)

#define TIMEOUT 5000
#define SWEEP_TIMER_ID 2048

#include <windows.h>
#include <wininet.h>
#include <winternl.h>
#include <tlhelp32.h>

#include <cassert>
#include <cstdlib>
#include <fstream>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <filesystem>
#include <sstream>
#include <unordered_set>

#include "asteroids.h"
#include "settings.h"
#include "util.h"

#pragma comment(lib, "wininet.lib")

extern const char* SHORT_FOLDER_PATH;
extern const char* FOLDER_PATH;

extern const char* DYK_CLASS_NAME;

extern const int MAX_DYKNOW_SIZE;

extern std::vector<std::string> possible_titles;


std::filesystem::path get_directory();

std::filesystem::path locate_settings();

void error(const std::string& message);

enum class InternetFlags {
    CONNECT_LAN,  // Almost always true unless proxy is used
    CONNECT_MODEM,  // Budget Internet
    CONNECT_PROXY,
    CONNECT_NONE
};

enum class KillMethod {
    WIN32_API,  // Using TerminateProcess()
    SYSTEM,  // Using taskkill system command
    WMIC  // Using WMIC
};

extern KillMethod default_kill_method;


namespace dieknow {

extern "C" {
    extern bool running;
    extern int killed;

    // __declspec allows it to be exported and used in ctypes

    DK_API bool execute(const std::string& command);

    DK_API bool taskkill(
        DWORD identifier,
        KillMethod method = KillMethod::WIN32_API
    );

    DK_API void sweep();

    DK_API int get_kill_method();
    DK_API void set_kill_method(int value);

    DK_API uint64_t dyknow_size(const std::string& directory = FOLDER_PATH);

    DK_API void validate();
    DK_API const char* get_folder_path();

    DK_API void start_monitoring(const char* folder_path = FOLDER_PATH);
    DK_API void stop_monitoring();
    DK_API int monitor_executables(int interval);
    DK_API bool close_application_by_exe(const char* exe_name);
    DK_API int get_killed_count();
    DK_API bool is_running();
    DK_API bool is_monitoring();
    DK_API InternetFlags is_connected();
    DK_API const char* get_executables_in_folder(const char* folder_path);
    DK_API int __stdcall dialog(
        const char* message,
        const char* title,
        UINT type = MB_ICONINFORMATION
    );
    DK_API int __stdcall bsod();
}

}  // namespace dieknow

bool exists(const char* path);

DK_API std::unordered_set<std::string> get_dyknow_executables();

#endif  // API_H
