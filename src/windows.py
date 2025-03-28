"""Retrieve information from DyKnow window."""

import win32gui
import win32api
import win32con
import win32process


def get_window_info(title):
    """Retreive information from given window."""

    hwnd = win32gui.FindWindow(None, "Task Manager")
    if not hwnd:
        print(f"Window with title '{title}' not found.")
        return

    rect = win32gui.GetWindowRect(hwnd)
    x, y, width, height = \
        rect[0], rect[1], \
        rect[2] - rect[0], \
        rect[3] - rect[1]

    style = win32api.GetWindowLong(hwnd, win32con.GWL_STYLE)
    ex_style = win32api.GetWindowLong(hwnd, win32con.GWL_EXSTYLE)

    thread_id, process_id = win32process.GetWindowThreadProcessId(hwnd)

    is_visible = win32gui.IsWindowVisible(hwnd)
    is_enabled = win32gui.IsWindowEnabled(hwnd)

    class_name = win32gui.GetClassName(hwnd)

    print(f"Window Handle: {hwnd}")
    print(f"Class Name: {class_name}")
    print(f"Position: ({x}, {y})")
    print(f"Size: {width}x{height}")
    print(f"Style: {hex(style)}")
    print(f"Extended Style: {hex(ex_style)}")
    print(f"Thread ID: {thread_id}")
    print(f"Process ID: {process_id}")
    print(f"Is Visible: {is_visible}")
    print(f"Is Enabled: {is_enabled}")

    try:
        import psutil
        process = psutil.Process(process_id)
        print(f"Process Name: {process.name()}")
        print(f"Executable Path: {process.exe()}")
    except ImportError:
        print("Did not find psutil... skipping process details.")

    text = win32gui.GetWindowText(hwnd)
    print(f"Window Text: {text}")

    print()
    print("Child windows")
    print("=============")

    def enum_child_windows_callback(hwnd, target_name):
        """Enumerate through child windows and retreive information."""
        class_name = win32gui.GetClassName(hwnd)
        window_text = win32gui.GetWindowText(hwnd)

        if not window_text: return

        rect = win32gui.GetWindowRect(hwnd)

        is_enabled = win32gui.IsWindowEnabled(hwnd)

        style = win32gui.GetWindowLong(hwnd, win32con.GWL_STYLE)
        ex_style = win32gui.GetWindowLong(hwnd, win32con.GWL_EXSTYLE)

        thread_id, process_id = win32process.GetWindowThreadProcessId(hwnd)

        print(f"  Window class: {class_name}")
        print(f"    ID: {hwnd}")
        print(f"    Text: '{window_text}'")
        print(f"    Position: {rect[:2]}")
        print(f"    Size: {rect[2:]}")
        print(f"    Enabled: {is_enabled}")
        print(f"    Style: {style:#010x}")
        print(f"    Extended Style: {ex_style:#010x}")
        print(f"    Process ID: {process_id}")
        print(f"    Thread ID: {thread_id}")

    win32gui.EnumChildWindows(hwnd, enum_child_windows_callback, None)

get_window_info("Do you understand?")
get_window_info("Health check")
