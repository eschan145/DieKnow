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
FILENAME: src/asteroids.h
DESCRIPTION: Asteroids hider for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-27
VERSION: 2.0.1
*/

#include <windows.h>
#include <shellapi.h>
#include <tchar.h>

#include <iostream>


#define WM_TRAYICON 1064
#define ID_TRAY_EXIT 1065

extern NOTIFYICONDATA nid;

struct Rect {
    float x;
    float y;
    float width;
    float height;
};

class Asteroids {
 public:
    bool is_ready = false;
    Rect rect;
    HWND hwnd;

    static LRESULT CALLBACK TrayWindowProc(
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam
    );

    void add();

    void create_menu();

    void create(bool& flag);

    void kill();
};

void create(bool& running);
