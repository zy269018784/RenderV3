#include <Util/String.h>
#include <fstream>
#include <codecvt>
#include <locale>

#define UTF8PROC_STATIC
#include <utf8proc.h>

namespace Render
{

    // utf16 ת utf8
    std::string UTF8FromUTF16(std::u16string str) {
        std::wstring_convert<
            std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>,
            char16_t>
            cnv;
        std::string utf8 = cnv.to_bytes(str);
        return utf8;
    }

    // utf8 ת utf16
    std::u16string UTF16FromUTF8(std::string str) {
        std::wstring_convert<
            std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>,
            char16_t>
            cnv;
        std::u16string utf16 = cnv.from_bytes(str);
        return utf16;
    }

#ifdef RENDER_IS_WINDOWS
    // u16 ת wstring
    std::wstring WStringFromU16String(std::u16string str) {
        std::wstring ws;
        ws.reserve(str.size());
        for (char16_t c : str)
            ws.push_back(c);
        return ws;
    }

    // utf8 ת wstring
    std::wstring WStringFromUTF8(std::string str) {
        return WStringFromU16String(UTF16FromUTF8(str));
    }

    //  wstring ת u16
    std::u16string U16StringFromWString(std::wstring str) {
        std::u16string su16;
        su16.reserve(str.size());
        for (wchar_t c : str)
            su16.push_back(c);
        return su16;
    }

    //  wstring ת u8
    std::string UTF8FromWString(std::wstring str) {
        return UTF8FromUTF16(U16StringFromWString(str));
    }
#endif

    std::string NormalizeUTF8(std::string str) {
        utf8proc_option_t options = UTF8PROC_COMPOSE;

        utf8proc_uint8_t* result;
        utf8proc_ssize_t length = utf8proc_map((const unsigned char*)str.data(), str.size(),
            &result, options);
        if (length < 0)
            printf("Unicode normalization error: %s: \"%s\"", utf8proc_errmsg(length), str);

        str = std::string(result, result + length);
        free(result);
        return str;
    }
}