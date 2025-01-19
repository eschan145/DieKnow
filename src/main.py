"""DieKnow main interface.
"""

import faulthandler
import re
import dieknow


faulthandler.enable()


def main():
    """Main starting point."""

    dieknow.validate()

    print()
    print("DieKnow Shell\n=============")

    while True:
        user_input = input(">>> ").strip().lower()

        if "help" in user_input:
            match = re.match(
                r"\bhelp\b(?:\s+(\w+))?",
                user_input.strip(),
                re.IGNORECASE)
            if match and match.group(1):
                attr = getattr(dieknow, match.group(1), None)
                if attr:
                    title = "Documentation for the function %s at %s:" % \
                            (match.group(1), attr)
                    print(title)
                    print("=" * len(title), "\n")
                    print(attr.__doc__)
                else:
                    print("Unknown command!")
            else:
                print("Welcome to DieKnow's help utility!\n\nType \"help\" "
                      "and then a function name below to get started!\n\n"
                      "Ex. for help on the function \"validate\", type \"help "
                      "validate\".")
                attrs = [attr for attr in dir(dieknow.shell) \
                    if not attr.startswith("__")]
                column_width = (len(attrs) + 1) // 2
                left_column = attrs[:column_width]
                right_column = attrs[column_width:]

                # Print each column side by side
                for left, right in zip(
                    left_column,
                    right_column + [""] * \
                        (len(left_column) - len(right_column))):
                    print(f"{left:<30} {right}")

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

            case "method":
                value = input("Enter the new termination method (0-2): ")
                if (value >= 0) and (value <= 2):
                    dieknow.set_kill_method(value);
                elif not value:
                    print(dieknow.get_kill_method())
                else:
                    print("Invalid input!")

            case "exit":
                if dieknow.status():
                    dieknow.stop_monitoring()
                print("Exiting")
                break

            case _:
                if not "help" in user_input:
                    func = getattr(dieknow.shell, user_input, None)

                    if func:
                        try:
                            if callable(func):
                                result = func()
                            else:
                                result = func
                            if result is not None:
                                print(result)
                        except (TypeError, AttributeError):
                            pass
                    else:
                        print("Invalid name!")


if __name__ == "__main__":
    main()
