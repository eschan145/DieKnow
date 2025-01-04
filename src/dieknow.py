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

md = False
if "-docs" in sys.argv:
    md = True

lib_dll_path = os.path.join(os.path.dirname(__file__), "dlls", "api.dll")

lib = ctypes.CDLL(lib_dll_path)

lib.validate.argtypes = None
lib.validate.restype = None
lib.get_folder_path.argtypes = None
lib.get_folder_path.restype = ctypes.c_char_p
lib.start_monitoring.argtypes = [ctypes.c_char_p]
lib.start_monitoring.restype = None
lib.close_application_by_exe.restype = ctypes.c_bool
lib.get_killed_count.restype = ctypes.c_int
lib.get_executables_in_folder.argtypes = [ctypes.c_char_p]
lib.get_executables_in_folder.restype = ctypes.c_char_p
lib.is_running.restype = ctypes.c_bool
lib.bsod.restype = ctypes.c_int
lib.dialog.argtypes = [wintypes.LPCWSTR, wintypes.LPCWSTR, wintypes.UINT]
lib.dialog.restype = ctypes.c_int

validate = lib.validate
folder_path = lib.get_folder_path()
start_monitoring = lib.start_monitoring
stop_monitoring = lib.stop_monitoring
monitor_executables = lib.monitor_executables
get_killed_count = lib.get_killed_count
close_application_by_exe = lib.close_application_by_exe
get_executables_in_folder = lib.get_executables_in_folder
is_running = lib.is_running
bsod = lib.bsod
dialog = lib.dialog

doc.doc(os.path.join(os.path.dirname(__file__), "api.cpp"), lib, markdown=md)

gui_dll_path = os.path.join(os.path.dirname(__file__), "dlls", "gui.dll")

guilib = ctypes.CDLL(gui_dll_path)
create_window = guilib.create_window

# Aliases
gui = create_window
directory = get_executables_in_folder
start = start_monitoring
stop = stop_monitoring
status = is_running

del (
    ctypes,
    doc,
    gui_dll_path,
    guilib,
    is_running,
    lib,
    lib_dll_path,
    md,
    os,
    sys,
    wintypes
)
