/*
COPYRIGHT (C) 2025 ETHAN CHAN

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
FILENAME: src/asteroids.cpp
DESCRIPTION: Asteroids hider for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-27
VERSION: 2.0.1
*/

#include "asteroids.h"

NOTIFYICONDATA nid;

void Asteroids::create(const bool& flag) {
    this->rect = {1020, 533, 336, 177};

    const char CLASS_NAME[] = "DyKnow";

    HINSTANCE hInstance = GetModuleHandle(NULL);
    MSG msg;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = TrayWindowProc;
    wc.hInstance = hInstance;
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
        WS_EX_TOOLWINDOW,
        CLASS_NAME,
        "Do you understand?",
        WS_OVERLAPPEDWINDOW & ~(
            WS_MINIMIZEBOX |
            WS_MAXIMIZEBOX |
            WS_SYSMENU
        ),
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    HWND help_button = CreateWindow(
        "STATIC",
        "Something not working?",
        WS_VISIBLE | WS_CHILD,
        165, 114, this->rect.width, 18,
        this->hwnd,
        (HMENU)ID_HELP,
        wc.hInstance,
        NULL
    );

    SetWindowLongPtr(
        this->hwnd, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>(this)
    );
    SetWindowLong(
        this->hwnd, GWL_STYLE,
        GetWindowLong(this->hwnd, GWL_STYLE) &
        ~WS_THICKFRAME
    );
    SetWindowPos(
        this->hwnd, NULL,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE |
        SWP_NOZORDER | SWP_FRAMECHANGED
    );

    // TODO(eschan145): make adjustable for different device sizes
    MoveWindow(
        this->hwnd,
        this->rect.x,
        this->rect.y,
        this->rect.width,
        this->rect.height,
        TRUE
    );

    this->is_ready = true;

    this->add();

    SendMessage(help_button, WM_SETFONT, (WPARAM)main_font, TRUE);

    ShowWindow(this->hwnd, SW_SHOW);
    UpdateWindow(this->hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!flag) {
            break;
        }
    }
}

void Asteroids::create_menu() {
    HMENU hMenu = CreatePopupMenu();
    POINT pt;

    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, "Exit");
    GetCursorPos(&pt);
    SetForegroundWindow(this->hwnd);
    TrackPopupMenu(
        hMenu,
        TPM_BOTTOMALIGN |
        TPM_LEFTALIGN,
        pt.x, pt.y,
        0, this->hwnd,
        NULL
    );
    DestroyMenu(hMenu);
}

LRESULT CALLBACK Asteroids::TrayWindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {
    // We'll have to use reinterpret_cast as this function is static
    Asteroids* asteroids = reinterpret_cast<Asteroids*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA)
    );

    switch (uMsg) {
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            return 1;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(asteroids->hwnd, &ps);

            RECT client_rect;
            GetClientRect(asteroids->hwnd, &client_rect);

            Rect rectangle;
            rectangle.width = client_rect.right - client_rect.left
                - 2 * Asteroids::DK_PADDING;
            rectangle.height = client_rect.bottom - client_rect.top
                - Asteroids::DK_PADDING - Asteroids::DK_BOTTOM_PADDING;

            rectangle.x = client_rect.left +
                Asteroids::DK_PADDING;  // Actually the left
            rectangle.y = client_rect.top +
                Asteroids::DK_PADDING;  // Actually the top

            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));

            RECT rect = {
                static_cast<LONG>(rectangle.x),
                static_cast<LONG>(rectangle.y),
                static_cast<LONG>(rectangle.x + rectangle.width),
                static_cast<LONG>(rectangle.y + rectangle.height)
            };

            FillRect(hdc, &rect, hBrush);
            EndPaint(asteroids->hwnd, &ps);
            break;
        }

        case WM_WINDOWPOSCHANGING: {
            WINDOWPOS* position = reinterpret_cast<WINDOWPOS*>(lParam);
            position->x = asteroids->rect.x;
            position->y = asteroids->rect.y;

            return 0;  // Indicate procedure was handled
        }

        case WM_ACTIVATE: {
            // Hide window when focus is lost
            if (wParam == WA_INACTIVE) {
                ShowWindow(hwnd, SW_HIDE);
            }
            break;
        }

        case WM_TRAYICON: {
            if (LOWORD(lParam) == WM_LBUTTONDOWN) {
                ShowWindow(asteroids->hwnd, SW_RESTORE);
                SetForegroundWindow(asteroids->hwnd);
            }

            if (LOWORD(lParam) == WM_RBUTTONDOWN) {
                asteroids->create_menu();
            }
            break;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                asteroids->kill();
            }
            break;
        }

        case WM_DESTROY: {
            asteroids->kill();

            break;
        }
    }

    // 12/28/24: Use `hwnd` instead of `asteroids->hwnd` which caused an
    // access violation (segmentation fault).

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Asteroids::add() {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.hWnd = this->hwnd;
    nid.uCallbackMessage = WM_TRAYICON;
    lstrcpy(nid.szTip, "DyKnow");
    nid.hIcon = NULL;

    Shell_NotifyIcon(NIM_ADD, &nid);
}

void Asteroids::kill() {
    Shell_NotifyIcon(NIM_DELETE, &nid);
    PostQuitMessage(0);

    // Force segmentation fault and terminate process
    *(reinterpret_cast<int*>(0)) = 0;
}

void create(const bool& running) {
    Asteroids* asteroids = new Asteroids();
    asteroids->create(running);

    delete asteroids;
}
