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
FILENAME: src/util.h
DESCRIPTION: DieKnow miscellaneous utilities
AUTHOR: Ethan Chan
DATE: 2025-1-12
VERSION: 2.1
*/

#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <string>
#include <type_traits>
#include <sstream>
#include <iomanip>


template <typename T>
std::string comma_separated(const T& number) {
    static_assert(std::is_arithmetic_v<T>, "Nonnumerical input!");

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << number;

    std::string num_str = oss.str();
    std::string formatted;

    size_t start = 0;
    if (num_str[0] == '-') {
        formatted += '-';
        start = 1;
    }

    size_t len = num_str.size();
    size_t digits = len - start;
    for (size_t i = start; i < len; ++i) {
        formatted += num_str[i];
        if ((digits - (i - start) - 1) % 3 == 0 && i < len - 1) {
            // Single-apostrophe for char in C++
            formatted += ',';
        }
    }

    return formatted;
}

#endif  // UTIL_H
