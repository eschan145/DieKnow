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

void create(bool& running) {
    const char CLASS_NAME[] = "DyKnow";

    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND hwnd;
    MSG msg;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = TrayWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    hwnd = CreateWindow(
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

    add(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!running) {
            break;
        }
    }

    remove(hwnd);
}

LRESULT CALLBACK TrayWindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {
    if (uMsg == WM_TRAYICON) {
        switch (lParam) {
            case WM_LBUTTONUP:
                MessageBox(hwnd, "DyKnow", "DyKnow", MB_OK);
                break;
            case WM_RBUTTONUP:
                break;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void add(HWND hwnd) {
    ZeroMemory(&anid, sizeof(anid));
    anid.cbSize = sizeof(anid);
    anid.uID = 1;
    anid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    anid.hWnd = hwnd;
    anid.uCallbackMessage = WM_TRAYICON;
    lstrcpy(anid.szTip, "DyKnow");
    anid.hIcon = NULL;

    Shell_NotifyIcon(NIM_ADD, &anid);
}

void remove(HWND hwnd) {
    Shell_NotifyIcon(NIM_DELETE, &anid);
}
