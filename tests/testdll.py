"""Test for all dependencies needed for dynamic linking."""

import os
import ctypes
import time

DLL_FOLDER = "./DLLs"
dll_files = [f for f in os.listdir(DLL_FOLDER) if f.endswith(".dll")]

SUCCESS = 0
FAILED = 0

start = time.time()

for dll in dll_files:
    try:
        ctypes.CDLL(os.path.join(DLL_FOLDER, dll))
        print(f"Loaded {dll} successfully..")
        SUCCESS += 1

    except OSError as e:
        print(f"Error loading {dll}: {e}")
        FAILED += 1

end = time.time()

print(f"\n\nLoaded {SUCCESS} of {SUCCESS + FAILED} DLL libraries in "
      "{round(end - start, 2)} seconds.")
