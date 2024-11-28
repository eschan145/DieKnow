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
FILENAME: src/system.h
DESCRIPTION: System information for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-13
VERSION: 1.0.1
*/

#ifndef SYSTEM_H
#define SYSTEM_H

#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>

extern const double WINDOW_DELAY;
extern const std::vector<std::string> WINDOW_EXCLUDE_LIST;

struct Window {
    HWND hwnd;
    std::string title;
    std::string class_name;

    bool operator==(const Window& other) const;
};

bool is_valid(const char* title);

std::string get_cpu_name();

std::string get_gpu_name();

std::string get_os_info();

std::string get_avaliable_ram();

void press(BYTE key);

void release(BYTE key);

inline void push(BYTE key);

void toggle_internet();

BOOL CALLBACK enum_windows(HWND hwnd, LPARAM lParam)

#endif // SYSTEM_H
