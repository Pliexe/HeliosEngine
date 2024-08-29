#pragma once
#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios::conversions
{
	std::string HELIOS_API from_u8string(const std::u8string& str);

#ifdef HELIOS_PLATFORM_WINDOWS
	std::wstring HELIOS_API from_utf8_to_utf16(const std::string& str);
	std::wstring HELIOS_API from_utf8_to_utf16(const std::u8string& str);
	std::string HELIOS_API from_utf16_to_utf8(const std::wstring& str);
	//std::wstring from_u8string(const std::u8string& str);
#endif
}