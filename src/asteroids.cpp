#include "asteroids.h"

void create() {
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
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.hWnd = hwnd;
    nid.uCallbackMessage = WM_TRAYICON;
    lstrcpy(nid.szTip, "DyKnow");
    nid.hIcon = NULL;

    Shell_NotifyIcon(NIM_ADD, &nid);
}

void remove(HWND hwnd) {
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
