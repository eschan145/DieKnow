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

extern const char* FOLDER_PATH;

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
