#pragma once
#include <string>
namespace Render {
    struct FileLoc {
        FileLoc() = default;
        FileLoc(std::string_view filename) : filename(filename) {}

        std::string_view filename;
        int line = 1, column = 0;
    };
}