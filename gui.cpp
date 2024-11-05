#include <vector>
#include <string>
#include <windows.h>
#include <commctrl.h>

#include "api.cpp"

extern "C" {
    __declspec(dllexport) void create_window();
}

void tooltip(HWND hwnd, HWND control, const char* text) {
    HWND htooltip = CreateWindowEx(
        0, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd, NULL, NULL, NULL
    );
    
    TOOLINFO tool_info = {};
    tool_info.cbSize = sizeof(tool_info);
    tool_info.uFlags = TTF_SUBCLASS;
    tool_info.hwnd = control;
    tool_info.hinst = NULL;
    tool_info.lpszText = const_cast<LPSTR>(text);
    
    // Get dimensions of the control
    GetClientRect(control, &tool_info.rect);
    SendMessage(htooltip, TTM_ADDTOOL, 0, (LPARAM)&tool_info);
}

void write(const std::string& filename, int value) {
    std::ofstream file(filename);
    
    if (file.is_open()) {
        file << value;
        file.close();
    } else {
        MessageBox(nullptr, "Failed to open file", "Error", NULL);
    }
}


class Application {
public:
    std::vector<HWND> widgets;
    std::vector<std::string> previous_executables;

    Application() {
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
            "Segoe UI");

        HWND hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            "DieKnow",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, wc.hInstance, NULL);

        if (hwnd == NULL) {
            return;
        }

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        MoveWindow(hwnd, 0, 0, 410, 600, TRUE);

        HWND running_button = CreateWindow(
            "BUTTON",
            "Start",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10,
            10,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            hwnd,
            (HMENU)Widgets::RUNNING,
            wc.hInstance,
            NULL);

        HWND exit_button = CreateWindow(
            "BUTTON",
            "Quit and Exit",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10,
            50,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            hwnd,
            (HMENU)Widgets::EXIT,
            wc.hInstance,
            NULL
        );
        HWND directory = CreateWindow(
            "LISTBOX",
            nullptr,
            WS_VISIBLE | WS_CHILD | LBS_STANDARD,
            BUTTON_WIDTH + 20,
            10,
            BUTTON_WIDTH + 60,
            170,
            hwnd,
            (HMENU)Widgets::DIRECTORY,
            wc.hInstance,
            NULL
        );
        HWND interval_label = CreateWindow(
            "STATIC",
            "Interval:",
            WS_VISIBLE | WS_CHILD,
            10,
            158 + BUTTON_HEIGHT,
            50, 18,
            hwnd,
            NULL,
            wc.hInstance,
            NULL
        );
        HWND interval_edit = CreateWindow(
            "EDIT",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            100,
            156 + BUTTON_HEIGHT,
            50, 22,
            hwnd,
            (HMENU)Widgets::INTERVAL,
            wc.hInstance,
            NULL
        );
        HWND interval_set = CreateWindow(
            "BUTTON",
            "Set interval",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            230,
            150 + BUTTON_HEIGHT,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            hwnd,
            (HMENU)Widgets::INTERVAL_SET,
            wc.hInstance,
            NULL
        );
        widgets.push_back(running_button);
        widgets.push_back(exit_button);
        widgets.push_back(directory);
        widgets.push_back(interval_label);
        widgets.push_back(interval_edit);
        widgets.push_back(interval_set);

        tooltip(hwnd, running_button, "Toggle between DieKnow running or stopped");
        tooltip(hwnd, exit_button, "Exit the DieKnow application and terminate all processes");
        tooltip(hwnd, directory, "Directory of the DyKnow files");
        tooltip(hwnd, interval_edit, "Delay between ticks for closing DyKnow");
        tooltip(hwnd, interval_set, "Set the interval between ticks for closing DyKnow");

        for (HWND widget : widgets) {
            SendMessage(widget, WM_SETFONT, (WPARAM)main_font, TRUE);
        }

        SetTimer(hwnd, 1, 50, nullptr);

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        Application* app = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        switch (uMsg) {
            case WM_COMMAND:
                if (LOWORD(wParam) == Widgets::RUNNING) {
                    if (running) {
                        stop_monitoring();
                    } else {
                        start_monitoring(FOLDER_PATH);
                    }

                    std::string status = running ? "Stop" : "Start";
                    SetWindowText(app->widgets[Widgets::RUNNING], status.c_str());
                }

                if (HIWORD(wParam) == EN_KILLFOCUS && (HWND)wParam == app->widgets[Widgets::INTERVAL]) {
                    char buffer[16];

                    GetWindowText(app->widgets[Widgets::INTERVAL], buffer, sizeof(buffer));
                    int value = atoi(buffer);
                    MessageBox(nullptr, "writing", NULL, NULL);
                    if (value > 0) {
                        write("interval.txt", value);
                    }
                }

                if (LOWORD(wParam) == Widgets::EXIT) {
                    DestroyWindow(hwnd);
                }
                break;

            case WM_CHAR:
                if (GetFocus() == app->widgets[Widgets::INTERVAL]) {
                    if (wParam == VK_RETURN) {
                        SetFocus(NULL);
                        return 0;
                    }
                    if ((wParam < '1' || wParam > '9') && wParam != VK_BACK) {
                        return 0; // Ignore non-numeric or zero input
                    }
                }

                break;

            case WM_TIMER:
                if (wParam == 1) {
                    app->update();
                }
                return 0;

            case WM_DESTROY:
                KillTimer(hwnd, 1);
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    void update() {
        std::vector<std::string> current_executables;

        for (const auto& entry : fs::directory_iterator(FOLDER_PATH)) {
            if (entry.is_regular_file() && entry.path().extension() == ".exe") {
                current_executables.push_back(entry.path().filename().string());
            }
        }

        if (current_executables == previous_executables) {
            return;
        }

        previous_executables = current_executables;

        SendMessage(widgets[Widgets::DIRECTORY], LB_RESETCONTENT, 0, 0);

        for (const std::string& file_name : current_executables) {
            SendMessage(widgets[Widgets::DIRECTORY], LB_ADDSTRING, 0, (LPARAM)file_name.c_str());
        }
    }
};

void create_window() {
    Application* application = new Application();
    delete application;
}
