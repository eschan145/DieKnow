import os
import time
import threading

import win32api
import win32con
import psutil
import pywintypes

running = False


class Command:
    commands = [
        "start",
        "stop",
        "help",
        "exit"
    ]

    def __getattribute__(self, name):
        if name in super().__getattribute__("commands"):
            return name

        if name == "commands":
            return super().__getattribute__("commands")

        return None


commands = Command()


def get_executables_in_folder(folder_path):
    exec_names = []
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            if file.endswith(".exe"):
                exec_names.append(file)
    return exec_names


def close_application_by_exe(exe_name):
    for proc in psutil.process_iter():
        try:
            if proc.name().lower() == exe_name.lower():
                handle = win32api.OpenProcess(
                    win32con.PROCESS_TERMINATE, False, proc.pid
                )
                win32api.TerminateProcess(handle, 0)
                win32api.CloseHandle(handle)
                # print(f"{exe_name} terminated.")
                return True

        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass

        except pywintypes.error:
            pass

    return False


def monitor_executables(folder_path):
    global running
    executables = get_executables_in_folder(folder_path)

    while running:
        # print("Monitoring and terminating executables...")
        for exe in executables:
            close_application_by_exe(exe)
        time.sleep(0.01)


def start_monitoring(folder_path):
    global running
    if not running:
        running = True
        monitor_thread = threading.Thread(
            target=monitor_executables, args=(folder_path,)
        )
        monitor_thread.start()
        print("Monitoring started...")


def stop_monitoring():
    global running
    if running:
        running = False
        print("Monitoring stopped...")


def main():
    folder_path = r"C:/Program Files/DyKnow/Cloud/7.10.22.9"

    while True:
        user_input = input(">>> ")

        match user_input.lower():
            case commands.start:
                start_monitoring(folder_path)
            case commands.stop:
                stop_monitoring()
                # break
            case commands.help:
                print(f"{commands.commands}")

            case commands.exit:
                stop_monitoring()
                break
            case _:
                print(f"Invalid input. Avaliable inputs:{commands.commands}")


if __name__ == "__main__":
    main()
