import os
import win32api
import win32con
import psutil
import threading
import time
import pywintypes

running = False

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
                handle = win32api.OpenProcess(win32con.PROCESS_TERMINATE, False, proc.pid)
                win32api.TerminateProcess(handle, 0)
                win32api.CloseHandle(handle)
                # print(f"{exe_name} terminated.")
                return True

        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass
    
        except (pywintypes.error):
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
        monitor_thread = threading.Thread(target=monitor_executables, args=(folder_path,))
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
            case "start":
                start_monitoring(folder_path)
            case "stop":
                stop_monitoring()
                # break
            case "exit":
                stop_monitoring()
                break
            case _:
                print("Invalid input. Avaliable inputs:START\nSTOP")

if __name__ == "__main__":
    main()
