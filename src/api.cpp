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
FILENAME: src/api.cpp
DESCRIPTION: API functions for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-13
VERSION: 1.0.1

Compile with g++ -shared -o api.dll api.cpp -Ofast -fPIC -shared
*/

#include "api.h"

const char* FOLDER_PATH = "C:\\Program Files\\DyKnow\\Cloud";


void validate() {
    if (!std::filesystem::exists(FOLDER_PATH)) {
        std::ostringstream msg;
        msg << "A DyKnow installation was not able to be found on your device.\n"
            << "Ensure the folder \"" << FOLDER_PATH
            << "\" exists and you have the permissions to access it!\n\n"
            << "Additionally, ensure you have one of the supported DyKnow versions. "
            << "You may need to upgarde your DieKnow to a later version.";

        MessageBox(nullptr, msg.str().c_str(), "FATAL ERROR", MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
}

bool exists(const char* path) {
    /*
    Check if a filepath exists.
    */

    DWORD ftyp = GetFileAttributesA(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (ftyp & FILE_ATTRIBUTE_DIRECTORY);
}

void close_application_by_exe(const char* exe_name) {
    /*
    Close a Windows PE executable file given the executable name.
    */

    // Create a snapshot of all running process(es)
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Break out if the snapshot failed
    if (hProcessSnap == INVALID_HANDLE_VALUE) return;

    // Iterate through the process list and terminate them as desired
    if (Process32First(hProcessSnap, &pe32)) {
        // Populate `pe32` with process snapshot information
        do {
            // Check if the executable name is the one given as a parameter
            if (_stricmp(pe32.szExeFile, exe_name) == 0) {
                // Open a HANDLE to the process. This is a little ambiguous as
                // it appears we are opening the process.
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    // Destroy the process handle to avoid memory leaks
                    CloseHandle(hProcess);
                    killed++;
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    // Destroy the snapshot to avoid memory leaks
    CloseHandle(hProcessSnap);
}

void monitor_executables(const char* folder_path) {
    /*
    Begin monitoring and closing of the executables in the given folder path.
    */

    while (running) {
        // Search recursively through folder_path and terminate all "*.exe"s
        for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
            // If it's a directory, go through its subfiles
            if (entry.is_directory()) {
                for (const auto& sub_entry : std::filesystem::directory_iterator(entry.path())) {
                    if ((sub_entry.is_regular_file()) &&
                        (sub_entry.path().extension() == ".exe")) {
                        close_application_by_exe(sub_entry.path().filename().string().c_str());
                    }
                }
            }
            else {
                validate();
            }
        }

        int interval;
        std::ifstream interval_file("../interval.txt");

        if (interval_file.is_open()) {
            interval_file >> interval;

            if (interval_file.fail()) {
                interval = 0;
            }

            // Minimize CPU usage
            std::this_thread::sleep_for(std::chrono::seconds(interval));
            interval_file.close();
        }
    }
}

const char* get_folder_path() {
    return FOLDER_PATH;
}

void start_monitoring(const char* folder_path = FOLDER_PATH) {
    /*
    Begin monitoring executables.
    */

    if (!running) {
        running = true;

        std::thread thread(monitor_executables, folder_path);
        HANDLE handle = reinterpret_cast<HANDLE>(thread.native_handle());

        // Reduces CPU usage by prioritizing other applications.
        // Other options:
        // * IDLE - only run when its the only thread
        // * LOWEST
        // * BELOW_NORMAL
        // * NORMAL - default priority
        // * ABOVE_NORMAL
        // * HIGHEST
        // * TIME_CRITICAL

        SetThreadPriority(handle, THREAD_PRIORITY_BELOW_NORMAL);

        // Detach thread from main and start it
        thread.detach();
    }
}

void stop_monitoring() {
    /*
    Stop monitoring executables.
    */

    // Although just a variable is set to false, because the DieKnow process is
    // in a separate thread it will finish immediately.

    running = false;
}

// Both get_killed_count and is_running must be declared as functions as ctypes
// does not support retrieving variables.

int get_killed_count() {
    /*
    Retrieve the amount of DyKnow executables killed.
    */

    return killed;
}

bool is_running() {
    /*
    Check if DieKnow is running or not.
    */

    return running;
}

const char* get_executables_in_folder(const char* folder_path) {
    /*
    Retrieve a printable list of executables in a folder.
    */

    static std::string result;
    result.clear();

    for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
        // If it's a directory, go through its subfiles
        if (entry.is_directory()) {
            for (const auto& sub_entry : std::filesystem::directory_iterator(entry.path())) {
                if ((sub_entry.is_regular_file()) &&
                    (sub_entry.path().extension() == ".exe")) {
                    // Add newline to print out nicely
                    result += sub_entry.path().filename().string() + "\n";
                }
            }
        }
        else {
            validate();
        }
    }

    return result.c_str();
}

__declspec(dllexport) int __stdcall bsod() {
    /*
    Open the Windows Blue Screen of Death via win32api's `NtRaiseHardError`.
    */

    BOOLEAN bEnabled;
    ULONG uResp;

    // Load RtlAdjustPrivilege and NtRaiseHardError functions from ntdll.dll
    auto RtlAdjustPrivilege = (NTSTATUS(WINAPI*)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN))
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlAdjustPrivilege");

    auto NtRaiseHardError = (NTSTATUS(WINAPI*)(NTSTATUS, ULONG, ULONG, PULONG_PTR, ULONG, PULONG))
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtRaiseHardError");

    if (!RtlAdjustPrivilege || !NtRaiseHardError) {
        return -1;
    }

    // Enable shutdown privilege for this process
    RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);

    // Trigger BSOD
    NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, nullptr, 6, &uResp);

    return 0;
}
