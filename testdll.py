"""Test for all dependencies needed for dynamic linking."""

import os
import ctypes
import time

dll_folder = "./DLLs"
dll_files = [f for f in os.listdir(dll_folder) if f.endswith(".dll")]

success = 0
failed = 0

start = time.time()

for dll in dll_files:
    try:
        ctypes.CDLL(os.path.join(dll_folder, dll))
        print(f"Loaded {dll} successfully.")
        success += 1

    except OSError as e:
        print(f"Error loading {dll}: {e}")
        failed += 1

end = time.time()

print(f"\n\nLoaded {success} of {success + failed} DLL libraries in "
      "{round(end - start, 2)} seconds.")
