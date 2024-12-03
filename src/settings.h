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
FILENAME: src/settings.h
DESCRIPTION: Setting configuration for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-13
VERSION: 1.0.1
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <unordered_map>
#include <string>

class Settings {
    std::unordered_map<std::string, std::string> settings;
    std::string path;

public:
    bool load(const std::string& file_name);

    template <typename T>
    T get(const std::string& key, T default_value = T()) const;

    bool set(const std::string& key, const std::string& value);
    void print() const;
    void update();
};

template <>
bool Settings::get<bool>(const std::string& key, bool default_value) const;

#endif // SETTINGS_H
