"""DieKnow main interface.
"""

import dieknow


def main():
    """Main starting point."""

    dieknow.validate()

    print("DieKnow Shell\n=============")

    while True:
        user_input = input(">>> ").strip().lower()

        if user_input == "start":
            if not dieknow.is_running():
                dieknow.start_monitoring()
                print("Monitoring started...")
            else:
                dieknow.dialog(
                    "The DieKnow process has already been started!",
                    "Information",
                    dieknow.MB_ICONINFORMATION
                )

        elif user_input == "stop":
            if not dieknow.is_running():
                dieknow.dialog(
                    "The DieKnow process has already been stopped and is not "
                    "running!",
                    "Information",
                    dieknow.MB_ICONINFORMATION
                )
            else:
                dieknow.stop_monitoring()
                print("Monitoring stopped...")

        elif user_input == "count":
            killed = dieknow.get_killed_count()
            print(f"Executables killed: {killed}")
        elif user_input == "directory":
            executables = dieknow.get_executables_in_folder(
                dieknow.folder_path()
            )
            print(f"Files in {dieknow.folder_path().decode()}:")
            print(executables.decode())
        elif user_input == "gui":
            dieknow.create_window()
        elif user_input == "bsod":
            print("Opening blue screen of death...")
            dieknow.bsod()
        elif user_input == "exit":
            dieknow.stop_monitoring()
            break
        else:
            print("Invalid input. Available inputs: start, stop, count, directory, exit")


if __name__ == "__main__":
    main()
