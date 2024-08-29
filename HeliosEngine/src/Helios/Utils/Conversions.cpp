#include "Conversions.h"

namespace Helios
{
	std::string conversions::from_u8string(const std::u8string& str)
	{
		return std::string(str.begin(), str.end());
	}
	std::wstring conversions::from_utf8_to_utf16(const std::string& str)
	{
		int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		std::wstring result(requiredSize, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], requiredSize);
		return result;
	}
	std::wstring conversions::from_utf8_to_utf16(const std::u8string& str)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(from_u8string(str));
	}
	std::string conversions::from_utf16_to_utf8(const std::wstring& str)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(str);
	}
}
