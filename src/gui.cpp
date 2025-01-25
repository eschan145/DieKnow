/*
COPYRIGHT (C) 2024 ETHAN CHAN

ALL RIGHTS RESERVED. UNAUTHORIZED COPYING, MODIFICATION, DISTRIBUTION, OR USE
OF THIS SOFTWARE WITHOUT PRIOR PERMISSION IS STRICTLY PROHIBITED.

THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM,
OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

PROJECT NAME: DieKnow
FILENAME: src/gui.cpp
DESCRIPTION: GUI interface to DieKnow API
AUTHOR: Ethan Chan
DATE: 2024-11-13
VERSION: 1.0.1
*/

#include "gui.h"


void tooltip(HWND hwnd, HWND control, const char* text) {
    /*
    Display a tooltip to aid user interactions.
    */

    HWND tooltip = CreateWindowEx(
        0, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd, NULL, NULL, NULL
    );

    SetWindowText(tooltip, "Tooltip");

    TOOLINFO tool_info = {};
    tool_info.cbSize = sizeof(tool_info);
    tool_info.uFlags = TTF_SUBCLASS;
    tool_info.hwnd = control;
    tool_info.hinst = NULL;
    tool_info.lpszText = const_cast<LPSTR>(text);

    // Get dimensions of the control
    GetClientRect(control, &tool_info.rect);
    SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM)&tool_info);
}

const char* get_selected(HWND listbox) {
    /*
    Get the selected item in a listbox.
    */

    int index = SendMessage(listbox, LB_GETCURSEL, 0, 0);
    if (index == LB_ERR) {
        // Unable to get listbox contents for some reason (no selection?)
        return "";
    }

    // Needed to specify memory allocation
    int length = SendMessage(listbox, LB_GETTEXTLEN, index, 0);

    // Create a character buffer to output the selected item
    char* buffer = new char[length + 1];
    SendMessage(listbox, LB_GETTEXT, index, (LPARAM)buffer);

    const char* text = _strdup(buffer);

    // Cleanup
    delete[] buffer;

    return text;
}

COLORREF Application::GREEN = RGB(0, 255, 0);
COLORREF Application::RED = RGB(255, 0, 0);

Application::Application() {
    dieknow::validate();

    // Used for help popup balloon
    InitCommonControls();

    const char CLASS_NAME[] = "DieKnow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = Application::WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HFONT main_font = CreateFont(
        18,
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        "Segoe UI"
    );

    this->hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "DieKnow",
        WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX),
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL, NULL,
        wc.hInstance,
        NULL
    );

    if (this->hwnd == NULL) {
        return;
    }

    SetWindowLongPtr(this->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // Resize the window
    MoveWindow(this->hwnd, 0, 0, (BUTTON_WIDTH * 2) + (10 * 5), 600, TRUE);

    std::string status = dieknow::is_running() ? "Stop" : "Start";

    HWND running_button = CreateWindow(
        "BUTTON",
        status.c_str(),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        PADDING,
        PADDING,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        this->hwnd,
        (HMENU)Widgets::RUNNING,
        wc.hInstance,
        NULL
    );

    HWND taskkill_button = CreateWindow(
        "BUTTON",
        "Terminate selected",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        PADDING,
        BUTTON_HEIGHT + (PADDING * 2),
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        this->hwnd,
        (HMENU)Widgets::TASKKILL,
        wc.hInstance,
        NULL
    );

    HWND exit_button = CreateWindow(
        "BUTTON",
        "Quit and Exit",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        PADDING,
        (BUTTON_HEIGHT * 2) + (PADDING * 3),
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        this->hwnd,
        (HMENU)Widgets::EXIT,
        wc.hInstance,
        NULL
    );
    HWND directory = CreateWindow(
        "LISTBOX",
        nullptr,
        WS_VISIBLE | WS_CHILD | LBS_STANDARD,
        BUTTON_WIDTH + (PADDING * 2),
        PADDING,
        BUTTON_WIDTH,
        170,
        this->hwnd,
        (HMENU)Widgets::DIRECTORY,
        wc.hInstance,
        NULL
    );
    HWND interval_label = CreateWindow(
        "STATIC",
        "Interval:",
        WS_VISIBLE | WS_CHILD,
        PADDING,
        120 + (BUTTON_HEIGHT * 2) + PADDING,
        50, 18,
        this->hwnd,
        (HMENU)Widgets::INTERVAL_LABEL,
        wc.hInstance,
        NULL
    );
    HWND interval_edit = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        40 + (PADDING * 2),
        120 + (BUTTON_HEIGHT * 2) + PADDING,
        50, 22,
        this->hwnd,
        (HMENU)Widgets::INTERVAL,
        wc.hInstance,
        NULL
    );
    HWND interval_set = CreateWindow(
        "BUTTON",
        "Set interval",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        BUTTON_WIDTH + (PADDING * 2),
        150 + BUTTON_HEIGHT,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        this->hwnd,
        (HMENU)Widgets::INTERVAL_SET,
        wc.hInstance,
        NULL
    );
    this->state = CreateWindow(
        "STATIC",
        "DyKnow state: ",
        WS_VISIBLE | WS_CHILD,
        PADDING,
        (BUTTON_HEIGHT * 3) + (PADDING * 4),
        BUTTON_WIDTH,
        18,
        this->hwnd,
        (HMENU)Widgets::STATE,
        wc.hInstance,
        NULL
    );
    HWND executables_killed = CreateWindow(
        "STATIC",
        "Executables terminated:",
        WS_VISIBLE | WS_CHILD,
        PADDING,
        (BUTTON_HEIGHT * 3) + (PADDING * 6),
        BUTTON_WIDTH, 18,
        this->hwnd,
        (HMENU)Widgets::EXECUTABLES_KILLED,
        wc.hInstance,
        NULL
    );
    HWND open_explorer = CreateWindow(
        "BUTTON",
        "Open in Explorer",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        BUTTON_WIDTH + (PADDING * 2),
        150 + (BUTTON_HEIGHT * 2) + PADDING,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        this->hwnd,
        (HMENU)Widgets::OPEN_EXPLORER,
        wc.hInstance,
        NULL
    );
    HWND display_information = CreateWindow(
        "BUTTON",
        "System information...",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        BUTTON_WIDTH + (PADDING * 2),
        150 + (BUTTON_HEIGHT * 3) + (PADDING * 2),
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        this->hwnd,
        (HMENU)Widgets::SYSTEM_INFORMATION,
        wc.hInstance,
        NULL
    );
    HWND take_snapshot = CreateWindow(
        "BUTTON",
        "Take snapshot",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        PADDING,
        130 + (BUTTON_HEIGHT * 2) + (PADDING * 3),
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        this->hwnd,
        (HMENU)Widgets::TAKE_SNAPSHOT,
        wc.hInstance,
        NULL
    );
    this->restore_snapshot = CreateWindow(
        "BUTTON",
        "Restore snapshot",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        PADDING,
        150 + (BUTTON_HEIGHT * 3) + (PADDING * 2),
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        this->hwnd,
        (HMENU)Widgets::RESTORE_SNAPSHOT,
        wc.hInstance,
        NULL
    );
    windows = CreateWindow(
        WC_LISTVIEW,
        nullptr,
        WS_VISIBLE | WS_CHILD | LVS_REPORT,
        PADDING,
        160 + (BUTTON_HEIGHT * 4) + (PADDING * 3),
        PADDING + (BUTTON_WIDTH * 2),
        200,
        this->hwnd,
        (HMENU)Widgets::WINDOWS,
        wc.hInstance,
        NULL
    );

    this->widgets.push_back(running_button);
    this->widgets.push_back(taskkill_button);
    this->widgets.push_back(exit_button);
    this->widgets.push_back(directory);
    this->widgets.push_back(interval_label);
    this->widgets.push_back(interval_edit);
    this->widgets.push_back(interval_set);
    this->widgets.push_back(this->state);
    this->widgets.push_back(executables_killed);
    this->widgets.push_back(open_explorer);
    this->widgets.push_back(display_information);
    this->widgets.push_back(take_snapshot);
    this->widgets.push_back(this->restore_snapshot);
    this->widgets.push_back(windows);

    tooltip(this->hwnd, running_button,
        "Toggle between DieKnow running or stopped."
    );
    tooltip(this->hwnd, taskkill_button,
        "Terminate the selected executable in the listbox."
    );
    tooltip(this->hwnd, exit_button,
        "Exit the DieKnow application and terminate all processes."
    );
    tooltip(this->hwnd, directory, "Directory of the DyKnow files.");
    tooltip(this->hwnd, interval_edit,
        "Delay between ticks for closing DyKnow."
    );
    tooltip(this->hwnd, interval_set,
        "Set the interval between ticks for closing DyKnow. Beware - an "
        "interval of 0 can saturate a CPU core. In milliseconds."
    );
    tooltip(this->hwnd, executables_killed,
        "Number of DyKnow executables terminated by DieKnow."
    );
    tooltip(this->hwnd, open_explorer,
        "Open the DyKnow file directory in the Windows Explorer."
    );
    tooltip(this->hwnd, display_information, "Show system information.");
    tooltip(this->hwnd, take_snapshot,
        "Take a snapshot of the current windows to restore them later on.");

    for (HWND widget : this->widgets) {
        SendMessage(widget, WM_SETFONT, (WPARAM)main_font, TRUE);
    }

    LVCOLUMN lv_title = {0};

    char title[] = "Window";

    lv_title.mask = LVCF_TEXT | LVCF_WIDTH;
    lv_title.pszText = title;
    lv_title.cx = 400;

    SendMessage(this->windows, LVM_INSERTCOLUMN, 0, (LPARAM)&lv_title);
    ListView_SetExtendedListViewStyle(
        this->windows,
        LVS_EX_CHECKBOXES |
        LVS_EX_GRIDLINES |
        LVS_EX_FULLROWSELECT
    );

    uint64_t size = dyknow_size();
    uint64_t size_in_mb = size / (1024 * 1024);

    if (size_in_mb > MAX_DYKNOW_SIZE) {
        std::string message = "Your DyKnow folder size was detected as "
            + comma_separated(size_in_mb) + "! If this continues, restart your "
            + "computer.";
        MessageBox(this->hwnd, message.c_str(), "Warning", MB_ICONWARNING);
    }

    // In ms -- set update rate to 10 ticks per second
    SetTimer(this->hwnd, 1, settings.get<int>("update", 100), nullptr);

    ShowWindow(this->hwnd, SW_SHOW);
    UpdateWindow(this->hwnd);

    MSG msg = {};
    // Needed to call WndProc system messages
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    this->hide_snapshots();
}

void Application::manage_command(
    Application* app,
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {
    /*
    Manage button commands in a switch statement.

    This function is called by `WindowProc`.
    */

    switch (LOWORD(wParam)) {
        case Widgets::RUNNING: {
            if (dieknow::running) {
                System::toggle_internet();
                dieknow::stop_monitoring();
                System::toggle_internet();
            } else {
                SetFocus(NULL);
                System::toggle_internet();
                dieknow::start_monitoring();
                System::toggle_internet();
            }

            std::string status = dieknow::running ? "Stop" : "Start";
            SetWindowText(app->widgets[Widgets::RUNNING], status.c_str());
            break;
        }

        case Widgets::TASKKILL: {
            // Check if the listbox has a selected item
            const char* selected = get_selected(
                app->widgets[Widgets::DIRECTORY]
            );

            // If it does, terminate its process
            if (selected && strlen(selected) > 0) {
                bool result = dieknow::close_application_by_exe(selected);

                if (result) {
                    std::string message = "Successfully closed " +
                                          std::string(selected);
                    MessageBox(
                        app->hwnd,
                        message.c_str(),
                        "Success",
                        MB_ICONINFORMATION
                    );
                } else {
                    std::string message = "Failed to close " +
                        std::string(selected) + "!. It may not be running.";

                    MessageBox(
                        app->hwnd,
                        message.c_str(),
                        "Failed",
                        MB_ICONERROR
                    );
                }
            } else {
                // Display an error if it doesn't
                MessageBox(
                    app->hwnd,
                    "Please select an item in the listbox.",
                    "Error",
                    MB_ICONERROR
                );
            }
            break;
        }

        case Widgets::INTERVAL_SET: {
            char buffer[16];

            GetWindowText(
                app->widgets[Widgets::INTERVAL],
                buffer, sizeof(buffer)
            );

            int value = atoi(buffer);

            if (value > 0) {
                settings.set("interval", std::to_string(value));

                std::string message = "Successfully set interval buffer to "
                                      + std::string(buffer);

                MessageBox(app->hwnd, message.c_str(), "Info", MB_ICONINFORMATION);
            }
            break;
        }

        case Widgets::OPEN_EXPLORER: {
            ShellExecute(NULL, "open", FOLDER_PATH, NULL, NULL, SW_SHOWDEFAULT);
            break;
        }

        case Widgets::SYSTEM_INFORMATION: {
            std::string cpu_name = System::get_cpu_name();
            std::string gpu_name = System::get_gpu_name();
            std::string os_info = System::get_os_info();
            std::string available_ram = System::get_available_ram();

            std::ostringstream message;
            message << "SYSTEM INFORMATION" << "\n"
                    << "==================" << "\n"
                    << "CPU: " << cpu_name << "\n"
                    << "GPU: " << gpu_name << "\n"
                    << "Operating system: " << os_info << "\n"
                    << "Free RAM: " << available_ram;

            MessageBox(
                app->hwnd,
                message.str().c_str(),
                "System Information",
                MB_OK | MB_ICONINFORMATION
            );

            break;
        }

        case Widgets::TAKE_SNAPSHOT: {
            std::vector<System::Window> new_snapshot;

            BOOL window_enumeration = EnumWindows(
                System::enum_snapshot,
                reinterpret_cast<LPARAM>(&new_snapshot)
            );

            if (!window_enumeration) {
                std::ostringstream message;
                message << "Failed to enumerate through windows."
                        << "Error: " << GetLastError();

                MessageBox(
                    app->hwnd,
                    message.str().c_str(),
                    "Error",
                    MB_ICONERROR
                );
                break;
            }

            if (new_snapshot.empty()) {
                MessageBox(
                    app->hwnd,
                    "Failed to retrieve windows.",
                    "Error",
                    MB_ICONERROR
                );
                break;
            }

            if (!(new_snapshot == app->snapshot)) {
                app->snapshot = new_snapshot;

                std::ostringstream message;
                std::size_t count = new_snapshot.size();

                message << "Snapshot successfully taken with "
                        << count
                        << " window(s).";

                MessageBox(
                    app->hwnd,
                    message.str().c_str(),
                    "SUCCESS",
                    MB_ICONINFORMATION
                );
            } else {
                MessageBox(
                    app->hwnd,
                    "The new snapshot matches the contents of the previous "
                    "snapshot. No changes were made.",
                    "Information",
                    MB_ICONINFORMATION
                );
            }

            break;
        }

        case Widgets::RESTORE_SNAPSHOT: {
            app->is_restoring = !app->is_restoring;

            std::string status = app->is_restoring ?
                "Unrestore snapshots" : "Restore snapshots";

            SetWindowText(
                app->restore_snapshot,
                status.c_str()
            );

            if (app->is_restoring) {
                app->restore_snapshots();
            } else {
                app->hide_snapshots();
            }

            break;
        }

        case Widgets::EXIT: {
            DestroyWindow(app->hwnd);
            break;
        }
    }
}

LRESULT CALLBACK Application::WindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {
    /*
    Manage window events.

    This is called internally by the Windows API.
    */

    // We'll have to use reinterpret_cast as this function is static
    Application* app = reinterpret_cast<Application*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA)
    );

    switch (uMsg) {
        case WM_COMMAND:
            app->manage_command(app, hwnd, uMsg, wParam, lParam);
            break;

        case WM_ACTIVATE: {
            if ((wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE)) {
                app->update(hwnd, uMsg, wParam, lParam);
            }
            break;
        }

        // case WM_PAINT: {
        //     RECT rect;
        //     GetClientRect(app->hwnd, &rect);
        //     int width = rect.right - rect.left;
        //     int height = rect.bottom - rect.top;

        //     PAINTSTRUCT ps;
        //     HDC hdc = BeginPaint(app->hwnd, &ps);
        //     HDC buffer = CreateCompatibleDC(hdc);
        //     HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
        //     HBITMAP old_bitmap = (HBITMAP)SelectObject(buffer, bitmap);

        //     for (const auto& widget : app->widgets) {
        //         SendMessage(
        //             widget,
        //             WM_PRINT,
        //             (WPARAM)buffer,
        //             PRF_CLIENT |
        //             PRF_CHILDREN |
        //             PRF_NONCLIENT
        //         );
        //     }

        //     BitBlt(hdc, 0, 0, width, height, buffer, 0, 0, SRCCOPY);

        //     SelectObject(buffer, old_bitmap);
        //     DeleteObject(bitmap);
        //     DeleteDC(buffer);
        //     EndPaint(app->hwnd, &ps);

        //     break;
        // }

        // case WM_ERASEBKGND:
        //     return 1;

        case WM_CTLCOLORSTATIC: {
            auto label = (HWND)lParam;
            if (label == app->state) {
                HDC hdc_static = (HDC)wParam;
                COLORREF color = dieknow::is_monitoring() ?
                    Application::RED : Application::GREEN;

                SetTextColor(hdc_static, color);

                // Force redraw of the label to prevent overlapping
                // InvalidateRect(label, nullptr, TRUE);
                // UpdateWindow(label);

                return (LRESULT)GetStockObject(NULL_BRUSH);
            }
            break;
        }

        case WM_CHAR: {
            if (GetFocus() == app->widgets[Widgets::INTERVAL]) {
                if (wParam == VK_RETURN) {
                    SetFocus(NULL);
                    return 0;
                }
            }

            break;
        }

        case WM_TIMER:
            if (wParam == 1) {
                app->update(hwnd, uMsg, wParam, lParam);
            }
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Application::restore_snapshots() {
    /*
    Restore the saved window snapshot.

    There is no error handling if the snapshot is empty -- it is assumed that
    the snapshot restoration button will be disabled if the snapshot is empty
    instead.
    */

    this->is_restoring = true;

    // for (const auto& window : this->snapshot) {
    //     HWND hwnd = FindWindow(window.class_name.c_str(), nullptr);

    //     if (hwnd) {
    //         WNDPROC original = (WNDPROC)SetWindowLongPtr(
    //             hwnd,
    //             GWLP_WNDPROC,
    //             (LONG_PTR)System::ShieldWndProc
    //         );

    //         if (original) {
    //             System::original_procedures[hwnd] = original;
    //             std::cout << "Hooked: " << window.class_name << std::endl;
    //         } else {
    //             std::cerr << "Failed to hook: " << window.class_name
    //                       << " (" << GetLastError() << ")" << std::endl;
    //         }
    //     }
    // }
    // int success = 0;
    // int missing = 0;
    // int fail = 0;

    // for (const auto& window : this->snapshot) {
    //     HWND hwnd = FindWindow(window.class_name.c_str(), nullptr);

    //     if (hwnd) {
    //         ShowWindow(hwnd, SW_SHOW);

    //         if (IsWindowVisible(hwnd)) {
    //             success++;
    //         } else {
    //             fail++;
    //         }
    //     } else {
    //         missing++;
    //     }
    // }

    // std::ostringstream message;

    // message << "Of snapshot restoration: "
    //         << success << " successful, "
    //         << missing << " missing, and "
    //         << fail << " failed.";

    // MessageBox(this->hwnd, message.str().c_str(),
                  // "Information", MB_ICONINFORMATION);
}

void Application::hide_snapshots() {
    this->snapshot.clear();
    // for (const auto& [hwnd, original] : System::original_procedures) {
    //     SetWindowLongPtr(
    //         hwnd,
    //         GWLP_WNDPROC,
    //         (LONG_PTR)System::original_procedures[hwnd]
    //     );
    // }
    // System::original_procedures.clear();
    // std::cout << "All hooks removed." << std::endl;
}

inline void Application::update(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {
    /*
    Update display labels and listbox.

    * Update restore snapshot focus state
    * Update listbox if files have been changed
    * Update interval text based on settings configuration
    * Update label displaying executables terminated
    */

    if (this->is_restoring) {
        for (const auto& window : this->snapshot) {
            ShowWindow(window.hwnd, SW_SHOW);
        }
    }

    settings.update();

    if (IsIconic(this->hwnd))
        return;

    std::string state_message = std::string("DyKnow state: ") +
        (dieknow::is_monitoring() ? "True" : "False");

    SetWindowText(this->state, state_message.c_str());

    EnableWindow(this->restore_snapshot, !this->snapshot.empty());

    // Update directory listbox
    std::vector<std::string> current_executables;

    for (const auto& entry :
         std::filesystem::directory_iterator(FOLDER_PATH)) {
        if (entry.is_directory()) {
            for (const auto& sub_entry :
                 std::filesystem::directory_iterator(entry.path())) {
                if ((sub_entry.is_regular_file()) &&
                    (sub_entry.path().extension() == ".exe")) {
                    current_executables.push_back(
                        sub_entry.path()
                                 .filename()
                                 .string()
                    );
                }
            }
        }
    }

    if (!(current_executables == previous_executables)) {
        previous_executables = current_executables;

        SendMessage(widgets[Widgets::DIRECTORY], LB_RESETCONTENT, 0, 0);

        for (const std::string& file_name : current_executables) {
            SendMessage(
                widgets[Widgets::DIRECTORY],
                LB_ADDSTRING, 0,
                (LPARAM)file_name.c_str());
        }
    }

    // Update window shower listbox

    std::vector<System::Window> current_windows;

    EnumWindows(
        System::enum_windows,
        reinterpret_cast<LPARAM>(&current_windows)
    );

    // this->update_windows(current_windows);

    if (!(current_windows == previous_windows)) {
        previous_windows = current_windows;

        // Capture current scroll position, as it is reset in the next step
        SCROLLINFO si = {0};
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_POS | SIF_RANGE;

        GetScrollInfo(this->windows, SB_VERT, &si);

        int position = si.nPos;

        SendMessage(this->windows, LVM_DELETEALLITEMS, 0, 0);

        for (const auto& window : current_windows) {
            LVITEM item = {0};
            item.mask = LVIF_TEXT;
            item.iItem = ListView_GetItemCount(this->windows);
            item.pszText = const_cast<char*>(window.title.c_str());

            ListView_InsertItem(this->windows, &item);
        }

        // Restore previous scroll position

        int max_scroll = std::max(
            0, static_cast<int>(current_windows.size()) - 1
        );
        position = std::min(position, max_scroll);

        si.fMask = SIF_POS;
        si.nPos = position;
        SetScrollInfo(this->windows, SB_VERT, &si, TRUE);
        SendMessage(this->windows, LVM_SCROLL, 0, position);
    }


    // Update window visibility in listbox

    this->update_windows(current_windows);

    if ((GetFocus() != widgets[Widgets::INTERVAL]) &&
        (GetFocus() != widgets[Widgets::INTERVAL_SET])) {
        // Get cursor position to restore

        // Start and end of selection. If no text is selected it is the cursor
        // position.
        DWORD start = 0;
        DWORD end = 0;

        SendMessage(
            widgets[Widgets::INTERVAL],
            EM_GETSEL,
            (WPARAM)&start,
            (LPARAM)&end
        );

        int index = start;

        SetWindowText(
            widgets[Widgets::INTERVAL],
            std::to_string(settings.get<int>("interval", 0)).c_str()
        );

        // Restore cursor position. Without this the cursor position would
        // reset back when cleared

        int length = GetWindowTextLength(widgets[Widgets::INTERVAL]);

        // Clamp index to the current text length
        if (index < 0) index = 0;  // cppcheck-suppress knownConditionTrueFalse
        if (index > length) index = length;

        SendMessage(widgets[Widgets::INTERVAL], EM_SETSEL, index, index);
    }

    std::string message = "Executables terminated: " +
                          std::to_string(dieknow::get_killed_count());
    SetWindowText(
        widgets[Widgets::EXECUTABLES_KILLED],
        message.c_str()
    );

    // InvalidateRect(this->hwnd, nullptr, TRUE);
}

void Application::update_windows(
    const std::vector<System::Window>& current_windows) {
    /*
    Toggle visibility of windows based on the listbox.
    */

    for (size_t i = 0; i < current_windows.size(); ++i) {
        const auto& window = current_windows[i];
        HWND target = FindWindow(NULL, window.title.c_str());

        if (target) {
            int index = -1;
            for (int j = 0; j < ListView_GetItemCount(this->windows); ++j) {
                char text[256];
                ListView_GetItemText(this->windows, j, 0, text, sizeof(text));

                if (window.title == text) {
                    index = j;
                    break;
                }
            }

            if (index >= 0) {
                if (IsWindowVisible(target)) {
                    ListView_SetCheckState(this->windows, index, TRUE);
                } else {
                    ListView_SetCheckState(this->windows, index, FALSE);
                }
            }
        }
    }
}

DK_API void create_window() {
    /*
    Create the DieKnow GUI process.

    Unlike the command line process this does not start in an indepent thread!
    So ensure that you can close it easily after it finishes.
    */

    SetUnhandledExceptionFilter(System::ExceptionHandler);

    Application* application = new Application();

    std::cout << "Program size: "
              << sizeof(*application)
              << " bytes." << std::endl;

    delete application;
}
