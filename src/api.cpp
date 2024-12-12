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
Settings settings;


DK_API void validate() {
    /*
    Check for the validity of the DyKnow installation. If the DyKnow
    installation cannot be found, the application exits.

    Settings are loaded.
    */

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

    settings.load("./settings.conf");
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

bool close_application_by_exe(const char* exe_name) {
    /*
    Close a Windows PE executable file given the executable name.

    The win32 function `TerminateProcess()` is used, which has looser
    privilleges than `taskkill`. It's uncommon that it will require
    administrative permissions.
    */

    bool terminated = false;

    // Create a snapshot of all running process(es)
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Break out if the snapshot failed
    if (hProcessSnap == INVALID_HANDLE_VALUE) return false;

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

                    auto wait = [&]() {
                        DWORD result = WaitForSingleObject(hProcess, TIMEOUT);

                        switch (result) {
                            case WAIT_OBJECT_0:
                                terminated = true;
                                std::cout << "Process " << exe_name << " terminated successfully.\n";
                                break;

                            case WAIT_TIMEOUT:
                                std::cerr << "WaitForSingleObject timed out!\n";
                                break;

                            case WAIT_FAILED:
                                std::cerr << "Failed to terminate " << exe_name << "!\n";
                                break;

                            default:
                                break;
                        }
                    };

                    std::thread wait_thread(wait);
                    wait_thread.join();

                    // Destroy the process handle to avoid memory leaks
                    CloseHandle(hProcess);
                }
                else {
                    std::cerr << "Failed to open a handle to the process!";
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    // Destroy the snapshot to avoid memory leaks
    CloseHandle(hProcessSnap);

    if (terminated) killed++;

    return terminated;
}

void monitor_executables(const char* folder_path) {
    /*
    Begin monitoring and closing of the executables in the given folder path.

    A while loop will go through all of the executables in `FOLDER_PATH`. It
    will then attempt to terminate them individually. The folder path is
    refreshed each iteration of the loop.

    An interval that can be specified in settings controls how often the
    function is repeated. A low interval may cause high CPU usage while a low
    interval may give DyKnow ample time to start back up.

    If `FOLDER_PATH` cannot be validated, the `validate()` function is called.
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

        int interval = settings.get<int>("interval", 0);

        // Minimize CPU usage
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

DK_API const char* get_folder_path() {
    /*
    Retrieve the default DyKnow folder path.

    This is made into a function for use with ctypes.
    */

    return FOLDER_PATH;
}

DK_API void start_monitoring(const char* folder_path = FOLDER_PATH) {
    /*
    Begin monitoring executables.

    A separate thread is detached from the primary thread. This thread is set
    with a lower priority to reduce CPU usage.

    See `monitor_executables()`.
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
    else {
        std::cout << "The DieKnow process has already been started!";
    }
}

DK_API void stop_monitoring() {
    /*
    Stop monitoring executables.
    */

    // Although just a variable is set to false, because the DieKnow process is
    // in a separate thread it will finish immediately.

    if (running) {
        running = false;
    }
    else {
        std::cout << "The DieKnow process has already been stopped!";
    }
}

// Both get_killed_count and is_running must be declared as functions as ctypes
// does not support retrieving variables.

DK_API int get_killed_count() {
    /*
    Retrieve the amount of DyKnow executables killed.
    */

    return killed;
}

DK_API bool is_running() {
    /*
    Check if DieKnow is running or not.
    */

    return running;
}

DK_API const char* get_executables_in_folder(const char* folder_path) {
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

DK_API int __stdcall bsod() {
    /*
    Open the Windows Blue Screen of Death via win32api's `NtRaiseHardError`.

    Use with caution! Your system will freeze and shut down within a few
    seconds, losing any unsaved work.
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
