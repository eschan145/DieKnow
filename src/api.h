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
FILENAME: src/api.h
DESCRIPTION: API functions for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-13
VERSION: 1.0.1

Compile with g++ -shared -o api.dll api.cpp -Ofast -fPIC -shared
*/

#ifndef API_H
#define API_H

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <filesystem>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>

#include "settings.h"


extern const char* FOLDER_PATH;
extern Settings settings;


extern "C"
{
    bool running = false;
    int killed = 0;

    // __declspec allows it to be exported and used in ctypes

    __declspec(dllexport) void validate();
    __declspec(dllexport) const char* get_folder_path();
    __declspec(dllexport) void start_monitoring(const char* folder_path);
    __declspec(dllexport) void stop_monitoring();
    __declspec(dllexport) int get_killed_count();
    __declspec(dllexport) bool is_running();
    __declspec(dllexport) const char* get_executables_in_folder(const char* folder_path);
    __declspec(dllexport) int __stdcall dialog(
        LPCWSTR message,
        LPCWSTR title,
        UINT type
    )
    {
        return MessageBoxW(nullptr, message, title, type);
    }
    __declspec(dllexport) int __stdcall bsod();
}

void validate();

bool exists(const char* path);

void close_application_by_exe(const char* exe_name);

void monitor_executables(const char* folder_path);

#endif // API_H
