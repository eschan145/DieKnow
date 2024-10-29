"""DieKnow main interface.
"""

import os

import dieknow

def main():
    """Main starting point."""

    folder_path = b"C:/Program Files/DyKnow/Cloud/7.10.22.9"
    if not os.path.exists(folder_path):
        dieknow.dialog(
            "A DyKnow installation was not able to be found on your device. "
            "Ensure %s exists and you have the permissions to access it!"
            % folder_path, "FATAL ERROR",
            dieknow.MB_ICONERROR
        )
        exit()

    print("DieKnow Shell\n=============")

    while True:
        user_input = input(">>> ").strip().lower()

        if user_input == "start":
            dieknow.start_monitoring(folder_path)
            print("Monitoring started...")
        elif user_input == "stop":
            dieknow.stop_monitoring()
            print("Monitoring stopped...")
        elif user_input == "count":
            killed = dieknow.get_killed_count()
            print(f"Executables killed: {killed}")
        elif user_input == "directory":
            executables = dieknow.get_executables_in_folder(folder_path)
            print(f"Files in {folder_path.decode()}:")
            print(executables.decode())
        elif user_input == "bsod":
            dieknow.bsod()
        elif user_input == "exit":
            dieknow.stop_monitoring()
            break
        else:
            print("Invalid input. Available inputs: start, stop, count, directory, exit")

if __name__ == "__main__":
    main()
