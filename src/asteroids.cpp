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
FILENAME: src/asteroids.cpp
DESCRIPTION: Asteroids hider for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-27
VERSION: 2.0.1
*/

#include "asteroids.h"

NOTIFYICONDATA nid;

void Asteroids::create(bool& flag) {
    const char CLASS_NAME[] = "DyKnow";

    HINSTANCE hInstance = GetModuleHandle(NULL);
    MSG msg;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = TrayWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    this->hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "DyKnow",
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

    SetWindowLongPtr(this->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // TODO: make adjustable for different device sizes
    MoveWindow(this->hwnd, 1020, 533, 336, 177, TRUE);

    this->add();

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
        case WM_TRAYICON: {
            if (LOWORD(lParam) == WM_LBUTTONDOWN) {
                if (uMsg == WM_TRAYICON) {
                    MessageBox(hwnd, "DyKnow", "DyKnow", MB_OK);
                }
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

void create(bool& running) {
    Asteroids* asteroids = new Asteroids();
    asteroids->create(running);

    delete asteroids;
}
