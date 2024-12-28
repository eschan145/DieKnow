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

    this->hwnd = CreateWindow(
        wc.lpszClassName,
        "DyKnow",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    std::cout << "Successfully create asteroids HWND.\n";

    this->add();

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!flag) {
            break;
        }
    }
}

void Asteroids::create_menu() {
    std::cout << "Creating menu.\n";
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
        0, hwnd,
        NULL
    );
    DestroyMenu(hMenu);
    std::cout << "Created menu.\n";
}

LRESULT CALLBACK Asteroids::TrayWindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {
    std::cout << "Casting.\n";
    // We'll have to use reinterpret_cast as this function is static
    Asteroids* asteroids = reinterpret_cast<Asteroids*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA)
    );
    std::cout << "Casted!\n";

    if (uMsg == WM_TRAYICON) {
        switch (lParam) {
            case WM_LBUTTONUP:
                MessageBox(asteroids->hwnd, "DyKnow", "DyKnow", MB_OK);
                break;

            case WM_RBUTTONUP:
                asteroids->create_menu();
                break;

            case WM_COMMAND:
                if (LOWORD(wParam) == ID_TRAY_EXIT) {
                    Shell_NotifyIcon(NIM_DELETE, &nid);
                    PostQuitMessage(0);
                }
                break;

            case WM_DESTROY:
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
                break;
        }
    }
    return DefWindowProc(asteroids->hwnd, uMsg, wParam, lParam);
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

void create(bool& running) {
    Asteroids* asteroids = new Asteroids();
    asteroids->create(running);

    delete asteroids;
}
