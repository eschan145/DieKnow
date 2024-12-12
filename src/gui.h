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
FILENAME: src/gui.h
DESCRIPTION: GUI interface to DieKnow API
AUTHOR: Ethan Chan
DATE: 2024-11-13
VERSION: 1.0.1
*/

#ifndef GUI_H
#define GUI_H

#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <commctrl.h>
#include <unordered_map>

#include "api.cpp"
#include "system.cpp"
#include "settings.cpp"

// Or more correctly, widget dimensions
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 35;

// Space between widgets as padding
const int PADDING = 10;


namespace Widgets {
    enum Button {
        RUNNING = 0,
        TASKKILL,
        EXIT,
        DIRECTORY,
        INTERVAL_LABEL,
        INTERVAL,
        INTERVAL_SET,
        EXECUTABLES_KILLED,
        WINDOWS,
        OPEN_EXPLORER,
        SYSTEM_INFORMATION,
        TAKE_SNAPSHOT,
        RESTORE_SNAPSHOT,
    };
}


extern "C" {
    DK_API void create_window();
}

void tooltip(HWND hwnd, HWND control, const char* text);

void write(const std::string& filename, int value);

int read(const std::string& filename);

const char* get_selected(HWND listbox);

class Application {
public:
    // Used to call `WM_SETFONT`
    std::vector<HWND> widgets;

    // Used to determine whether or not to refresh the listbox
    std::vector<std::string> previous_executables;
    std::vector<Window> previous_windows;
    std::vector<Window> snapshot;

    HWND hwnd;
    HWND windows;
    HWND restore_snapshot;

    bool is_restoring = false;

    Application();

    void manage_command(Application* app, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Use `static` to allow it to be called as an event by Windows API
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void restore_snapshots();

    void hide_snapshots();

    inline void update(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void update_windows(std::vector<Window>& current_windows);
};

#endif // GUI_H
