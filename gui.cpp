#include <vector>
#include <string>
#include <windows.h>

#include "api.cpp"

extern "C" {
    __declspec(dllexport) void create_window();
}

class Application {
public:
    std::vector<HWND> widgets;
    std::vector<std::string> previous_executables;

    Application() {
        const char CLASS_NAME[] = "DieKnow";

        WNDCLASS wc = {};
        wc.lpfnWndProc = Application::WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
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
        MoveWindow(hwnd, 0, 0, 400, 600, TRUE);

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
            BUTTON_WIDTH * 1.5,
            200,
            hwnd,
            (HMENU)Widgets::DIRECTORY,
            wc.hInstance,
            NULL
        );
        widgets.push_back(running_button);
        widgets.push_back(exit_button);
        widgets.push_back(directory);

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
                if (LOWORD(wParam) == Widgets::EXIT) {
                    DestroyWindow(hwnd);
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
