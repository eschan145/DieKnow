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


std::filesystem::path get_directory() {
    char path[MAX_PATH];
    HMODULE hModule = nullptr;

    if (GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            (LPCSTR)&get_directory,
            &hModule
        )) {
        if (GetModuleFileNameA(hModule, path, MAX_PATH) > 0) {
            return std::filesystem::path(path).parent_path();
        } else {
            std::cerr << "Failed to locate DLL directory!\n";
        }
    }
    std::cerr << "Failed to locate DLL directory!\n";
}

std::filesystem::path locate_settings() {
    auto directory = get_directory();
    auto file_path = directory.parent_path().parent_path() / "settings.conf";
    if (std::filesystem::exists(file_path)) {
        std::cout << "Located settings.conf.\n";
        return file_path;
    } else {
        std::cerr << "Failed to locate settings.conf!\n";
        std::cerr << "It should be located at " << file_path << ".\n";
    }
}

extern "C" {
    bool running = false;
    int killed = 0;

    DK_API int __stdcall dialog(
        const char* message,
        const char* title,
        UINT type) {
        return MessageBox(nullptr, message, title, type);
    }
}

DK_API void validate() {
    /*
    Check for the validity of the DyKnow installation. If the DyKnow
    installation cannot be found, the application exits.

    Settings are loaded.
    */

    bool needs_exit = false;

    if (!std::filesystem::exists(FOLDER_PATH)) {
        needs_exit = true;

        std::ostringstream msg;
        msg << "A DyKnow installation was not able to be found on your "
            << "device.\n"
            << "Ensure the folder \"" << FOLDER_PATH
            << "\" exists and you have the permissions to access it!\n\n"
            << "Additionally, ensure you have one of the supported DyKnow "
            << "versions. "
            << "You may need to upgrade your DieKnow to a later version.";

        MessageBox(nullptr, msg.str().c_str(), "FATAL ERROR", MB_ICONERROR);
    } else {
        std::cout << "Successfully located DyKnow installation at "
                  << FOLDER_PATH << ".\n";
    }

    auto path = locate_settings();

    bool loaded_settings = settings.load(path.string());

    if (loaded_settings) {
        std::cout << "Successfully loaded DieKnow configuration files.\n";
    } else {
        std::cerr << "Failed to load DieKnow configuration files!\n";
        needs_exit = true;
    }

    if (needs_exit) {
        std::cerr << "FATAL: DyKnow validation failed!\n";
        std::exit(EXIT_FAILURE);
    } else {
        std::cout << "Successfully validated DyKnow installation and file "
                  << "integrity.\n";
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

DK_API bool close_application_by_exe(const char* exe_name) {
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

                // Request PROCESS_TERMIANTE right, which allows use to use
                // TerminateProcess. This may not work if the process is
                // elevated. Once we can obtain a handle to the process, we
                // can do anything we want with it under the permissions we
                // requested; e.g. terminating it.
                HANDLE hProcess = OpenProcess(
                    PROCESS_TERMINATE, FALSE,
                    pe32.th32ProcessID);
                if (hProcess) {
                    // Bam, terminated!
                    TerminateProcess(hProcess, 0);

                    auto wait = [&]() {
                        DWORD result = WaitForSingleObject(hProcess, TIMEOUT);

                        switch (result) {
                            case WAIT_OBJECT_0:
                                terminated = true;
                                std::cout << "Process " << exe_name
                                          << " terminated successfully.\n";
                                break;

                            case WAIT_TIMEOUT:
                                std::cerr << "WaitForSingleObject timed out!\n";
                                break;

                            case WAIT_FAILED:
                                break;

                            default:
                                break;
                        }
                    };

                    std::thread wait_thread(wait);
                    wait_thread.join();

                    // Destroy the process handle to avoid memory leaks
                    CloseHandle(hProcess);
                } else {
                    std::cerr << "Failed to open a handle to the process! "
                              << "Error code: " << GetLastError() << "\n";
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    // Destroy the snapshot to avoid memory leaks
    CloseHandle(hProcessSnap);

    if (terminated) killed++;

    return terminated;
}

DK_API int monitor_executables(const char* folder_path) {
    /*
    Begin monitoring and closing of the executables in the given folder path.

    A while loop will go through all of the executables in `FOLDER_PATH`. It
    will then attempt to terminate them individually. The folder path is
    refreshed each iteration of the loop.

    An interval that can be specified in settings controls how often the
    function is repeated. A low interval may cause high CPU usage while a low
    interval may give DyKnow ample time to start back up.

    If `FOLDER_PATH` cannot be validated, the `validate()` function is called.

    This function returns the amount of executables it terminated in this pass.

    > [!IMPORTANT]
    > It is best to call this in an independent thread, such as
    > `start_monitoring()` as this will run continuously until the variable
    > `running` is set to `false`.
    */

    int count = 0;

    while (running) {
        // Search recursively through folder_path and terminate all "*.exe"s
        for (const auto& entry :
             std::filesystem::directory_iterator(folder_path)) {
            // If it's a directory, go through its subfiles
            if (entry.is_directory()) {
                for (const auto& sub_entry :
                     std::filesystem::directory_iterator(entry.path())) {
                    if ((sub_entry.is_regular_file()) &&
                        (sub_entry.path().extension() == ".exe")) {
                        bool result = close_application_by_exe(
                            sub_entry.path().filename().string().c_str()
                        );
                        if (result)
                            count++;
                    }
                }
                break;

            } else {
                validate();
            }
        }

        int interval = settings.get<int>("interval", 0);

        // Minimize CPU usage
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
    return count;
}

DK_API const char* get_folder_path() {
    /*
    Retrieve the default DyKnow folder path.

    This is made into a function for use with ctypes.
    */

    return FOLDER_PATH;
}

DK_API void start_monitoring(const char* folder_path) {
    /*
    Begin monitoring executables.

    A separate thread is detached from the primary thread. This thread is set
    with a lower priority to reduce CPU usage.

    See `monitor_executables()`.
    */

    if (!running) {
        InternetFlags internet_state = is_connected();
        bool connected = true;

        switch (internet_state) {
            case InternetFlags::CONNECT_MODEM:
                std::cout << "Internet connected via modem.\n";
                break;
            case InternetFlags::CONNECT_LAN:
                std::cout << "Internet connected via LAN.\n";
                break;
            case InternetFlags::CONNECT_PROXY:
                std::cout << "Internet connected via proxy.\n";
                break;
            case InternetFlags::CONNECT_NONE:
                connected = false;
                std::cout << "Internet disconnected. Proceeding to start "
                          << "monitoring.\n";
                break;
        }

        if (connected) {
            std::cout << "Your Internet is detected as Connected. Please turn "
                      << "off or disable your Internet before you begin "
                      << "DieKnow! Once started, you can turn back on your "
                      << "Internet. Aborting.\n";
            return;
        }

        running = true;

        std::thread thread(monitor_executables, folder_path);
        HANDLE handle = reinterpret_cast<HANDLE>(thread.native_handle());

        std::cout << "Created monitoring std::thread and retrieved HANDLE.\n";

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

        std::cout << "SetThreadPriority() success: "
                  << "THREAD_PRIORITY_BELOW_NORMAL.\n";

        // Detach thread from main and start it
        thread.detach();
        std::cout << "Detatched monitoring thread.\n";
        std::cout << "Monitoring started.\n";

        std::thread asteroids_thread(create, std::ref(running));
        asteroids_thread.detach();
    } else {
        std::cout << "The DieKnow process has already been started!\n";
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
        std::cout << "Successfully stopped DieKnow process.\n";
    } else {
        std::cout << "The DieKnow process has already been stopped!\n";
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

DK_API InternetFlags is_connected() {
    /*
    Check for an Internet connection and return the connection type, one of
    `InternetFlags`.
    */

    DWORD flags;
    bool result = InternetGetConnectedState(&flags, 0);

    if (result) {
        if (flags & INTERNET_CONNECTION_MODEM)
            return InternetFlags::CONNECT_MODEM;
        if (flags & INTERNET_CONNECTION_LAN)
            return InternetFlags::CONNECT_LAN;
        if (flags & INTERNET_CONNECTION_PROXY)
            return InternetFlags::CONNECT_PROXY;
    }

    return InternetFlags::CONNECT_NONE;
}


DK_API const char* get_executables_in_folder(const char* folder_path) {
    /*
    Retrieve a printable list of executables in a folder.

    Ensures that the DyKnow installation can exist, and checks with
    `validate()` if it doesn't.
    */

    static std::string result;
    result.clear();

    bool found_dir = false;
    bool found_subfile = false;

    for (const auto& entry :
         std::filesystem::directory_iterator(folder_path)) {
        // If it's a directory, go through its subfiles
        if (entry.is_directory()) {
            for (const auto& sub_entry :
                 std::filesystem::directory_iterator(entry.path())) {
                if ((sub_entry.is_regular_file()) &&
                    (sub_entry.path().extension() == ".exe")) {
                    // Add newline to print out nicely
                    result += sub_entry.path().filename().string() + "\n";
                    found_subfile = true;
                }
            }
            if (found_subfile) {
                found_dir = true;
            }
            break;
        }
    }

    if (!found_dir) validate();

    return result.c_str();
}

DK_API int __stdcall bsod() {
    /*
    Open the Windows Blue Screen of Death via NT API's `NtRaiseHardError`.

    Use with caution! Your system will freeze and shut down within a few
    seconds, losing any unsaved work.

    This function `NtRaiseHardError` is part of the Windows New Technology API
    kernel and is completely undocumented. The reason why is to prevent
    non-system libraries from messing around with low-level settings they
    aren't supposed to.
    */

    BOOLEAN bEnabled;
    ULONG uResp;

    // Load RtlAdjustPrivilege and NtRaiseHardError functions from ntdll.dll
    auto RtlAdjustPrivilege = (NTSTATUS(WINAPI*)(
        ULONG,
        BOOLEAN,
        BOOLEAN,
        PBOOLEAN))GetProcAddress(
            GetModuleHandleW(L"ntdll.dll"),
            "RtlAdjustPrivilege"
    );

    auto NtRaiseHardError = (NTSTATUS(WINAPI*)(
        NTSTATUS,
        ULONG,
        ULONG,
        PULONG_PTR,
        ULONG,
        PULONG))GetProcAddress(
            GetModuleHandleW(L"ntdll.dll"),
            "NtRaiseHardError"
    );

    if (!RtlAdjustPrivilege || !NtRaiseHardError) {
        if (!RtlAdjustPrivilege) {
            std::cerr << "Failed to load RtlAdjustPrivilege from win32's "
                      << "nt.dll!\n";
        }
        if (!NtRaiseHardError) {
            std::cerr << "Failed to load NtRaiseHardError from win32's "
                      << "nt.dll!\n";
        }
        std::cerr << "Failed to open Windows BSOD!\n";
        return -1;
    }

    // Enable shutdown privilege for this process
    RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);

    if (bEnabled) {
        std::cout << "Successfully loaded permissions with "
                  << "RtlAdjustPrivilege().\n";
    }

    // Trigger BSOD
    NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, nullptr, 6, &uResp);

    std::cout << "Successfully initiated Windows BSOD. The system will "
              << "terminate in a few seconds...\n";

    return 0;
}
