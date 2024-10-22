#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <filesystem>
#include <atomic>

std::atomic<bool> running(false);
std::atomic<int> killed(0);

class Command
{
public:
    std::vector<std::string> commands = {
        "start",
        "stop",
        "directory",
        "count"
        "help"
        "exit"
    };

    std::string get_command(const std::string& name)
    {
        for (const auto& cmd : commands)
        {
            if (cmd == name)
            {
                return cmd;
            }
        }
        return "";
    }
};

Command commands;

std::vector<std::string> get_executables_in_folder(const std::string& folder_path)
{
    std::vector<std::string> exec_names;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_path))
    {
        if (entry.path().extension() == ".exe")
        {
            exec_names.push_back(entry.path().filename().string());
        }
    }
    return exec_names;
}

bool close_application_by_exe(const std::string& exe_name)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
        return false;

    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if (exe_name == pe32.szExeFile)
            {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                if (hProcess)
                {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    killed++;
                    CloseHandle(hProcessSnap);
                    return true;
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }
    CloseHandle(hProcessSnap);
    return false;
}

void monitor_executables(const std::string& folder_path)
{
    while (running)
    {
        std::vector<std::string> executables = get_executables_in_folder(folder_path);
        for (const auto& exe : executables)
        {
            close_application_by_exe(exe);
        }
        Sleep(1000);
    }
}

void start_monitoring(const std::string& folder_path)
{
    if (!running)
    {
        running = true;
        std::thread monitor_thread(monitor_executables, folder_path);
        monitor_thread.detach();
        std::cout << "Monitoring started...\n";
    }
}

void stop_monitoring()
{
    if (running)
    {
        running = false;
        std::cout << "Monitoring stopped...\n";
    }
}

int main()
{
    std::string folder_path = "C:/Program Files/DyKnow/Cloud/7.10.22.9";
    std::string input;
    std::cout << "DieKnow Shell\n=============\n";

    while (true)
    {
        std::cout << ">>> ";
        std::getline(std::cin, input);

        switch (input[0]) {
            case 's':
            {
                if (input == commands.get_command("start"))
                {
                    start_monitoring(folder_path);
                } else if (input == commands.get_command("stop"))
                {
                    stop_monitoring();
                }
                break;
            }
            case 'd':
            {
                if (input == commands.get_command("directory"))
                {
                    std::vector<std::string> execs = get_executables_in_folder(folder_path);
                    for (const auto& exe : execs)
                    {
                        std::cout << exe << "\n";
                    }
                }
                break;
            }
            case 'c':
            {
                if (input == commands.get_command("count"))
                {
                    std::cout << "Executables killed: " << killed.load() << "\n";
                }
                break;
            }
            case 'h':
            {
                if (input == commands.get_command("help"))
                {
                    for (const auto& cmd : commands.commands)
                    {
                        std::cout << cmd << "\n";
                    }
                }
                break;
            }
            case 'e':
            {
                if (input == commands.get_command("exit"))
                {
                    stop_monitoring();
                    return 0;
                }
                break;
            }
            default:
                std::cout << "Invalid input. Available inputs:";
                for (const auto& cmd : commands.commands)
                {
                    std::cout << " " << cmd;
                }
                std::cout << "\n";
        }
    }

    return 0;
}
