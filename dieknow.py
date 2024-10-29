"""DieKnow API.
"""

import ctypes
from ctypes import wintypes

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

lib = ctypes.CDLL("./api.dll")

lib.start_monitoring.argtypes = [ctypes.c_char_p]
lib.stop_monitoring.restype = None
lib.get_killed_count.restype = ctypes.c_int
lib.get_executables_in_folder.argtypes = [ctypes.c_char_p]
lib.get_executables_in_folder.restype = ctypes.c_char_p
lib.bsod.restype = ctypes.c_int
lib.dialog.argtypes = [wintypes.LPCWSTR, wintypes.LPCWSTR, wintypes.UINT]
lib.dialog.restype = ctypes.c_int

start_monitoring = lib.start_monitoring
stop_monitoring = lib.stop_monitoring
get_killed_count = lib.get_killed_count
get_executables_in_folder = lib.get_executables_in_folder
bsod = lib.bsod
dialog = lib.dialog
