"""DieKnow API.
"""


import ctypes
try:
    from ctypes import wintypes

except ImportError as exc:
    raise OSError("Failed to load Windows ctypes! Ensure you are on a Windows "
                  "platform!") from exc

import os
import sys

import doc

RED = "\033[91m"
RESET = "\033[0m"

md = False
if "-docs" in sys.argv:
    print("Documentation generation enabled")
    md = True

lib_dll_path = os.path.join(os.path.dirname(__file__), "dlls", "api.dll")
gui_dll_path = os.path.join(os.path.dirname(__file__), "dlls", "gui.dll")

try:
    lib = ctypes.CDLL(lib_dll_path)
    guilib = ctypes.CDLL(gui_dll_path)
except OSError as exc:
    error_message = (
        "OSError: "
        "Failed to load DLL libraries! Ensure that the library is "
        "not corrupted, it uses the same architecture (x64) as your "
        "machine (as well as its dependencies) and is not missing "
        "dependencies if dynamically linked!\n\nRefer to "
        "https://learn.microsoft.com/en-us/windows/win32/debug/"
        "system-error-codes for more information."
    )
    sys.stderr.write(f"{RED}{error_message}{RESET}\n")
    raise OSError from exc

try:
    lib.get_kill_method.argtypes = None
    lib.get_kill_method.restype = ctypes.c_int

    lib.set_kill_method.argtypes = [ctypes.c_int]
    lib.set_kill_method.restype = None

    lib.validate.argtypes = None
    lib.validate.restype = None
    lib.get_folder_path.argtypes = None
    lib.get_folder_path.restype = ctypes.c_char_p
    lib.start_monitoring.argtypes = [ctypes.c_char_p]
    lib.start_monitoring.restype = None
    lib.stop_monitoring.restype = None
    lib.close_application_by_exe.restype = ctypes.c_bool
    lib.get_killed_count.restype = ctypes.c_int
    lib.get_executables_in_folder.argtypes = [ctypes.c_char_p]
    lib.get_executables_in_folder.restype = ctypes.c_char_p
    lib.is_running.restype = ctypes.c_bool
    lib.is_monitoring.restype = ctypes.c_bool
    lib.bsod.restype = ctypes.c_int
    lib.dialog.argtypes = [wintypes.LPCWSTR, wintypes.LPCWSTR, wintypes.UINT]
    lib.dialog.restype = ctypes.c_int
except AttributeError as exc:
    parts = str(exc).split("'")
    function_name = parts[1] if len(parts) > 1 else "unknown"
    raise OSError(f"Function '{function_name}' name or ordinal missing!") from exc


for func_name in dir(lib):
    if not func_name.startswith("_"):
        globals()[func_name] = getattr(lib, func_name)

folder_path = lib.get_folder_path()

del func_name

doc.doc(os.path.join(os.path.dirname(__file__), "api.cpp"), lib, markdown=md)

try:
    create_window = guilib.create_window
except AttributeError as exc:
    parts = str(exc).split("'")
    function_name = parts[1] if len(parts) > 1 else "unknown"
    raise OSError(f"Function '{function_name}' name or ordinal missing!") from exc

# Aliases
gui = create_window
dir = get_executables_in_folder
start = start_monitoring
stop = stop_monitoring
status = is_running
dstatus = is_monitoring

class Shell:
    """DieKnow Shell commands."""

    def __init__(self):
        exclude = {
            "close_application_by_exe",
            "ctypes",
            "doc",
            "get_executables_in_folder",
            "get_folder_path",
            "get_killed_count",
            "create_window",
            "folder_path",
            "gui_dll_path",
            "guilib",
            "is_running",
            "is_monitoring",
            "lib",
            "lib_dll_path",
            "md",
            "RED",
            "RESET",
            "os",
            "start_monitoring",
            "stop_monitoring",
            "sys",
            "wintypes",
            "Shell",
            "get_kill_method",
            "set_kill_method"
        }
        for var_name, value in globals().items():
            if var_name not in exclude and not var_name.startswith("__"):
                setattr(self, var_name, value)

        self.folder = folder_path


shell = Shell()
