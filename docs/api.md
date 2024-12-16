# Documentation for `api.cpp`

### *function* `validate()`

Check for the validity of the DyKnow installation. If the DyKnow installation cannot be found, the application exits.

Settings are loaded.

**Signature**: `void`

### *function* `get_folder_path()`

Retrieve the default DyKnow folder path.

This is made into a function for use with ctypes.

**Signature**: `const char*`

### *function* `start_monitoring()`

Begin monitoring executables.

A separate thread is detached from the primary thread. This thread is set with a lower priority to reduce CPU usage.

See `monitor_executables()`.

**Signature**: `void`

### *function* `stop_monitoring()`

Stop monitoring executables.

**Signature**: `void`

### *function* `get_killed_count()`

Retrieve the amount of DyKnow executables killed.

**Signature**: `int`

### *function* `is_running()`

Check if DieKnow is running or not.

**Signature**: `bool`

### *function* `get_executables_in_folder()`

Retrieve a printable list of executables in a folder.

Ensures that the DyKnow installation can exist, and checks with `validate()` if it doesn't.

**Signature**: `const char*`

### *function* `bsod()`

Open the Windows Blue Screen of Death via NT API's `NtRaiseHardError`.

Use with caution! Your system will freeze and shut down within a few seconds, losing any unsaved work.

This function NtRaiseHardError is part of the Windows New Technology API kernel and is undocumented. The reason why is to prevent non-system libraries from messing around with low-level settings they aren't supposed to.

**Signature**: `int __stdcall`

