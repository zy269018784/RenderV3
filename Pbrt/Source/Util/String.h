#pragma once
#include <string>

namespace Render
{
	std::string UTF8FromUTF16(std::u16string str);
	std::u16string UTF16FromUTF8(std::string str);

#ifdef RENDER_IS_WINDOWS
	std::wstring WStringFromUTF8(std::string str);
	std::string UTF8FromWString(std::wstring str);

	std::wstring WStringFromU16String(std::u16string str);
	std::u16string U16StringFromWString(std::wstring str);
#endif  // Render2023_IS_WINDOWS

	std::string NormalizeUTF8(std::string str);
}