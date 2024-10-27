import ctypes

# Load the DLL
lib = ctypes.CDLL('./api.dll')

# Define the function signatures
lib.start_monitoring.argtypes = [ctypes.c_char_p]
lib.stop_monitoring.restype = None
lib.get_killed_count.restype = ctypes.c_int
lib.get_executables_in_folder.argtypes = [ctypes.c_char_p]
lib.get_executables_in_folder.restype = ctypes.c_char_p

def main():
    folder_path = b"C:/Program Files/DyKnow/Cloud/7.10.22.9"
    print("DieKnow Shell\n=============")

    while True:
        user_input = input(">>> ").strip().lower()

        if user_input == "start":
            lib.start_monitoring(folder_path)
            print("Monitoring started...")
        elif user_input == "stop":
            lib.stop_monitoring()
            print("Monitoring stopped...")
        elif user_input == "count":
            killed = lib.get_killed_count()
            print(f"Executables killed: {killed}")
        elif user_input == "directory":
            executables = lib.get_executables_in_folder(folder_path)
            print(f"Files in {folder_path.decode()}:")
            print(executables.decode())
        elif user_input == "exit":
            lib.stop_monitoring()
            break
        else:
            print("Invalid input. Available inputs: start, stop, count, directory, exit")

if __name__ == "__main__":
    main()
