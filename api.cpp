/*
Compile with g++ -shared -o api.dll api.cpp -Ofast -fPIC -shared
*/

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <filesystem>
#include <fstream>
#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>

using namespace std;
namespace fs = std::filesystem;

const char* FOLDER_PATH = "C:/Program Files/DyKnow/Cloud/7.10.22.9";
const int BUTTON_WIDTH = 150;
const int BUTTON_HEIGHT = 30;

namespace Widgets {
    enum Button {
        START = 1,
        STOP = 2
    };
}

extern "C"
{
    bool running = false;
    int killed = 0;

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
    __declspec(dllexport) void create_window();
}

void close_application_by_exe(const string& exe_name)
{
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (hProcessSnap == INVALID_HANDLE_VALUE) return;

    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if (_stricmp(pe32.szExeFile, exe_name.c_str()) == 0)
            {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                if (hProcess)
                {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    killed++;
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }
    CloseHandle(hProcessSnap);
}

void monitor_executables(const string& folder_path)
{
    while (running)
    {
        for (const auto& entry : fs::directory_iterator(folder_path))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".exe")
            {
                close_application_by_exe(entry.path().filename().string());
            }
        }

        int interval;
        std::ifstream interval_file("interval.txt");

        if (interval_file.is_open())
        {
            interval_file >> interval;

            if (interval_file.fail())
            {
                interval = 0;
            }

            this_thread::sleep_for(chrono::seconds(interval));
            interval_file.close();
        }
    }
}

void start_monitoring(const char* folder_path = FOLDER_PATH)
{
    if (!running)
    {
        running = true;
        thread(monitor_executables, string(folder_path)).detach();
    }
}

void stop_monitoring()
{
    running = false;
}

int get_killed_count()
{
    return killed;
}

bool is_running()
{
    return running;
}

const char* get_executables_in_folder(const char* folder_path)
{
    static string result;
    result.clear();

    for (const auto& entry : fs::directory_iterator(folder_path))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".exe")
        {
            result += entry.path().filename().string() + "\n";
        }
    }
    return result.c_str();
}

__declspec(dllexport) int __stdcall bsod()
{
    BOOLEAN bEnabled;
    ULONG uResp;

    // Load RtlAdjustPrivilege and NtRaiseHardError functions from ntdll.dll
    auto RtlAdjustPrivilege = (NTSTATUS(WINAPI*)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN))
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlAdjustPrivilege");

    auto NtRaiseHardError = (NTSTATUS(WINAPI*)(NTSTATUS, ULONG, ULONG, PULONG_PTR, ULONG, PULONG))
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtRaiseHardError");

    if (!RtlAdjustPrivilege || !NtRaiseHardError)
    {
        return -1;
    }

    // Enable shutdown privilege for this process
    RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);

    // Trigger BSOD
    NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, nullptr, 6, &uResp);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == Widgets::START) {
                start_monitoring(FOLDER_PATH);
            }
            else if (LOWORD(wParam) == Widgets::STOP) {
                stop_monitoring();
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void create_window() {
    const char CLASS_NAME[] = "DieKnow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "DieKnow",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, wc.hInstance, NULL
    );

    if (hwnd == NULL) {
        return;
    }

    HWND start_button = CreateWindow(
        "BUTTON",
        "Start monitoring",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        10,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        hwnd,
        (HMENU)Widgets::START,
        wc.hInstance,
        NULL);

    HWND stop_button = CreateWindow(
        "BUTTON",
        "Stop monitoring",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        50,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        hwnd,
        (HMENU)Widgets::STOP,
        wc.hInstance,
        NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
