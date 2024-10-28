"""DieKnow API.
"""

import ctypes

lib = ctypes.CDLL("./api.dll")

lib.start_monitoring.argtypes = [ctypes.c_char_p]
lib.stop_monitoring.restype = None
lib.get_killed_count.restype = ctypes.c_int
lib.get_executables_in_folder.argtypes = [ctypes.c_char_p]
lib.get_executables_in_folder.restype = ctypes.c_char_p

start_monitoring = lib.start_monitoring
stop_monitoring = lib.stop_monitoring
get_killed_count = lib.get_killed_count
get_executables_in_folder = lib.get_executables_in_folder
