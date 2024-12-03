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
FILENAME: src/settings.cpp
DESCRIPTION: Setting configuration for DieKnow
AUTHOR: Ethan Chan
DATE: 2024-11-13
VERSION: 1.0.1
*/

#include "settings.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cerrno>


bool Settings::load(const std::string& file_name) {
    std::ifstream file("../settings.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << file_name
                  << ". Reason: " << std::strerror(errno) << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if ((line.empty()) ||
            (line[0] == '#') ||
            (line.substr(0, 2) == "//")) continue;

        auto delimeter = line.find('=');
        if (delimeter == std::string::npos) continue;

        std::string key = line.substr(0, delimeter);
        std::string value = line.substr(delimeter + 1);
        settings[key] = value;
    }
    return true;
}

void Settings::print() const {
    for (const auto& [key, value] : settings) {
        std::cout << key << " = " << value << "\n";
    }
}

template <typename T>
T Settings::get(const std::string& key, T default_value) const {
    auto it = settings.find(key);
    if (it == settings.end()) return default_value;

    std::istringstream ss(it->second);
    T value;
    if (!(ss >> value)) {
        throw std::runtime_error("Invalid type for key: " + key);
    }
    return value;
}

template <>
bool Settings::get<bool>(const std::string& key, bool default_value) const {
    auto it = settings.find(key);
    if (it == settings.end()) return default_value;

    const std::string& value = it->second;
    if (value == "true" || value == "1") return true;
    if (value == "false" || value == "0") return false;

    return default_value;
}
