"""DieKnow API.
"""

import ctypes
from ctypes import wintypes

import os
import sys


# Button Options
MB_OK = 0x00000000
MB_OKCANCEL = 0x00000001
MB_YESNO = 0x00000004
MB_YESNOCANCEL = 0x00000003
MB_RETRYCANCEL = 0x00000005
MB_ABORTRETRYIGNORE = 0x00000002

# Icon Options
MB_ICONINFORMATION = 0x00000040
MB_ICONEXCLAMATION = 0x00000030
MB_ICONWARNING = MB_ICONEXCLAMATION
MB_ICONERROR = 0x00000010
MB_ICONQUESTION = 0x00000020

# Default Button Options
MB_DEFBUTTON1 = 0x00000000
MB_DEFBUTTON2 = 0x00000100
MB_DEFBUTTON3 = 0x00000200
MB_DEFBUTTON4 = 0x00000300


if sys.platform == "darwin":
    os.system('''
        osascript -e 'display dialog "DieKnow is only compatible with Windows \
        platforms!" with title "Fatal Error" buttons {{"OK"}} default button \
        "OK"'
    ''')
    sys.exit(1)
elif sys.platform.startswith("linux"):
    os.system(
        'zenity --info --title="Fatal Error" --text="DieKnow is only \
compatible with Windows platforms!"'
    )
    sys.exit(1)


lib_dll_path = os.path.join(os.path.dirname(__file__), "dlls", "api.dll")

lib = ctypes.CDLL(lib_dll_path)

lib.validate.argtypes = None
lib.validate.restype = None
lib.get_folder_path.argtypes = None
lib.get_folder_path.restype = ctypes.c_char_p
lib.start_monitoring.argtypes = [ctypes.c_char_p]
lib.stop_monitoring.restype = None
lib.get_killed_count.restype = ctypes.c_int
lib.get_executables_in_folder.argtypes = [ctypes.c_char_p]
lib.get_executables_in_folder.restype = ctypes.c_char_p
lib.is_running.restype = ctypes.c_bool
lib.bsod.restype = ctypes.c_int
lib.dialog.argtypes = [wintypes.LPCWSTR, wintypes.LPCWSTR, wintypes.UINT]
lib.dialog.restype = ctypes.c_int

validate = lib.validate
folder_path = lib.get_folder_path
start_monitoring = lib.start_monitoring
stop_monitoring = lib.stop_monitoring
get_killed_count = lib.get_killed_count
get_executables_in_folder = lib.get_executables_in_folder
is_running = lib.is_running
bsod = lib.bsod
dialog = lib.dialog


gui_dll_path = os.path.join(os.path.dirname(__file__), "dlls", "gui.dll")

guilib = ctypes.CDLL(gui_dll_path)
create_window = guilib.create_window
