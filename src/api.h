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

#include <windows.h>
#include <wininet.h>
#include <winternl.h>
#include <tlhelp32.h>

#include <cstdlib>
#include <fstream>

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <filesystem>
#include <sstream>

#include "asteroids.h"
#include "settings.h"

#pragma comment(lib, "wininet.lib")


extern const char* FOLDER_PATH;


std::filesystem::path get_directory();

std::filesystem::path locate_settings();


enum class InternetFlags {
    CONNECT_MODEM,
    CONNECT_LAN,
    CONNECT_PROXY,
    CONNECT_NONE
};


extern "C" {
    extern bool running;
    extern int killed;

    // __declspec allows it to be exported and used in ctypes

    DK_API void validate();
    DK_API const char* get_folder_path();
    DK_API void start_monitoring(const char* folder_path = FOLDER_PATH);
    DK_API void stop_monitoring();
    DK_API int monitor_executables(const char* folder_path = FOLDER_PATH);
    DK_API bool close_application_by_exe(const char* exe_name);
    DK_API int get_killed_count();
    DK_API bool is_running();
    DK_API InternetFlags is_connected();
    DK_API const char* get_executables_in_folder(const char* folder_path);
    DK_API int __stdcall dialog(
        const char* message,
        const char* title,
        UINT type = MB_ICONINFORMATION
    );
    DK_API int __stdcall bsod();
}

bool exists(const char* path);

#endif  // API_H
