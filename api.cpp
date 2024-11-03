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

std::vector<HWND> widgets;

const char* FOLDER_PATH = "C:\\Program Files\\DyKnow\\Cloud\\7.10.22.9";
const int BUTTON_WIDTH = 150;
const int BUTTON_HEIGHT = 30;

namespace Widgets {
    enum Button {
        RUNNING = 0,
        EXIT,
        DIRECTORY
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

bool exists(const char* path) {
    DWORD ftyp = GetFileAttributesA(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (ftyp & FILE_ATTRIBUTE_DIRECTORY);
}

void close_application_by_exe(const char* exe_name)
{
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (hProcessSnap == INVALID_HANDLE_VALUE) return;

    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if (_stricmp(pe32.szExeFile, exe_name) == 0)
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

void monitor_executables(const char* folder_path)
{
    while (running)
    {
        for (const auto& entry : fs::directory_iterator(folder_path))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".exe")
            {
                close_application_by_exe(entry.path().filename().string().c_str());
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
        thread(monitor_executables, folder_path).detach();
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

void update() {
    SendMessage(widgets[Widgets::DIRECTORY], LB_RESETCONTENT, 0, 0);

    for (const auto& entry : fs::directory_iterator(FOLDER_PATH)) {
        if (entry.is_regular_file() && entry.path().extension() == ".exe") {
            std::string file_name = entry.path().filename().string();
            SendMessage(widgets[Widgets::DIRECTORY], LB_ADDSTRING, 0, (LPARAM)file_name.c_str());
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == Widgets::RUNNING) {
                if (running) {
                    stop_monitoring();
                } else {
                    start_monitoring(FOLDER_PATH);
                }

                std::string status = running ? "Stop" : "Start";
                SetWindowText(widgets[Widgets::RUNNING], status.c_str());
            }
            if (LOWORD(wParam) == Widgets::EXIT) {
                DestroyWindow(hwnd);
            }
            break;

        case WM_TIMER:
            if (wParam == 1) {
                update();
            }
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
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

    HFONT main_font = CreateFont(
        18,
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        "Segoe UI");

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "DieKnow",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, wc.hInstance, NULL);

    if (hwnd == NULL) {
        return;
    }

    MoveWindow(hwnd, 0, 0, 400, 600, TRUE);

    HWND running_button = CreateWindow(
        "BUTTON",
        "Start",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        10,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        hwnd,
        (HMENU)Widgets::RUNNING,
        wc.hInstance,
        NULL);

    HWND exit_button = CreateWindow(
        "BUTTON",
        "Quit and Exit",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        10,
        50,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        hwnd,
        (HMENU)Widgets::EXIT,
        wc.hInstance,
        NULL
    );
    HWND directory = CreateWindow(
        "LISTBOX",
        nullptr,
        WS_VISIBLE | WS_CHILD | LBS_STANDARD,
        BUTTON_WIDTH + 20,
        10,
        BUTTON_WIDTH * 1.5,
        200,
        hwnd,
        (HMENU)Widgets::DIRECTORY,
        wc.hInstance,
        NULL
    );
    widgets.push_back(running_button);
    widgets.push_back(exit_button);
    widgets.push_back(directory);

    for (HWND widget : widgets) {
        SendMessage(widget, WM_SETFONT, (WPARAM)main_font, TRUE);
    }

    SetTimer(hwnd, 1, 50, nullptr);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
