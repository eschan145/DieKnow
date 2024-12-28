#include <windows.h>
#include <shellapi.h>
#include <tchar.h>

#define WM_TRAYICON 64

LRESULT CALLBACK TrayWindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
);

void add(HWND hwnd);

void remove(HWND hwnd);

void create(bool& flag);

NOTIFYICONDATA nid;
