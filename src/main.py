"""DieKnow main interface.
"""

import faulthandler
import dieknow


faulthandler.enable()


def main():
    """Main starting point."""

    dieknow.validate()

    print("DieKnow Shell\n=============")

    while True:
        user_input = input(">>> ").strip().lower()

        match user_input:
            case "start":
                dieknow.start_monitoring(dieknow.folder_path)

            case "directory":
                executables = dieknow.get_executables_in_folder(
                    dieknow.folder_path
                )
                print(f"Files in {dieknow.folder_path.decode('utf-8')}:")
                print(executables.decode())

            case "count":
                killed = dieknow.get_killed_count()
                print(f"Executables killed: {killed}")

            case "exit":
                dieknow.stop_monitoring()
                break

            case _:
                func = getattr(dieknow, user_input, None)

                if func:
                    try:
                        if callable(func): result = func()
                        else: result = func
                        if result:
                            print(result)
                        else:
                            print("Invalid input!")
                    except (TypeError, AttributeError):
                        pass
                else: print("Invalid input!")


if __name__ == "__main__":
    main()
