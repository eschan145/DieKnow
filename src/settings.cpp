/*
COPYRIGHT (C) 2025 ETHAN CHAN

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
    /*
    Load settings from a file name.

    The filename is retained to allow `update()` to refresh it.
    */

    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error when loading settings: Could not open file "
                  << file_name
                  << ". Reason: " << std::strerror(errno) << std::endl;
        return false;
    }

    this->path = file_name;

    std::string line;
    // Skip empty lines and lines that start with # or // as comments
    while (std::getline(file, line)) {
        if ((line.empty()) ||
            (line[0] == '#') ||
            (line.substr(0, 2) == "//")) continue;

        // Delimiter between key and value
        auto delimiter = line.find('=');
        if (delimiter == std::string::npos) continue;

        std::string key = line.substr(0, delimiter);
        std::string value = line.substr(delimiter + 1);
        settings[key] = value;
    }

    return true;
}

// cppcheck-suppress unusedFunction
void Settings::print() const {
    /*
    Print to the console a comprehensive list of keys and values in the
    settings file.
    */

    for (const auto& [key, value] : settings) {
        std::cout << key << " = " << value << "\n";
    }
}

template <typename T>
T Settings::get(const std::string& key, const T& default_value) const {
    /*
    Retrieve a value from the settings.

    This is a TEMPLATE - meaning you'll need to specify the type of the setting
    being accessed. For example:

    ```cpp
    settings.get<int>("count", 0)
    ```

    The default value is used if the setting cannot be found.
    */

    auto it = settings.find(key);
    if (it == settings.end()) {
        std::cerr << "Invalid type or value of key for " << key << "! "
                  << "The default value " << default_value
                  << " will be used instead.";
        return default_value;
    }

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

bool Settings::set(const std::string& key, const std::string& value) {
    settings[key] = value;

    if (this->path.empty()) return false;

    std::ofstream file(this->path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << this->path
                  << ". Reason: " << std::strerror(errno) << std::endl;
        return false;
    }

    for (const auto& [key, value] : settings) {
        file << key << "=" << value << "\n";
    }

    return true;
}

void Settings::update() {
    this->load(this->path);
}

template int Settings::get<int>(
    const std::string& key,
    int default_value
) const;
template double Settings::get<double>(
    const std::string& key,
    double default_value
) const;
template std::string Settings::get<std::string>(
    const std::string& key,
    std::string default_value
) const;

Settings settings;
