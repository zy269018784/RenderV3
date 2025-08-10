#pragma once
#include <string>
namespace Render {
    class InternedString {
    public:
        // InternedString Public Methods
        InternedString() = default;
        InternedString(const std::string* str) : str(str) {}
        operator const std::string& () const { return *str; }

        bool operator==(const char* s) const { return *str == s; }
        bool operator==(const std::string& s) const { return *str == s; }
        bool operator!=(const char* s) const { return *str != s; }
        bool operator!=(const std::string& s) const { return *str != s; }
        bool operator<(const char* s) const { return *str < s; }
        bool operator<(const std::string& s) const { return *str < s; }

        std::string ToString() const { return *str; }

    private:
        const std::string* str = nullptr;
    };
}