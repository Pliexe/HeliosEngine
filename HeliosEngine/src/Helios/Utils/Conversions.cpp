#include "Conversions.h"

#ifdef _WIN32
#include <windows.h>  // Windows-specific headers
#elif defined(__EMSCRIPTEN__)
//#include <emscripten.h>  // WebAssembly-specific headers (if needed)
#else
#include <cstdlib>  // For mbstowcs (Linux and POSIX systems)
#include <unicode/unistr.h>
#endif

namespace Helios
{
	std::string conversions::from_u8string(const std::u8string& str)
	{
		return std::string(str.begin(), str.end());
	}
	std::u8string conversions::from_string(const std::string& str)
	{
		return std::u8string(str.begin(), str.end());
	}
	std::wstring conversions::from_utf8_to_utf16(const std::string& str)
	{
		std::wstring result;

#ifdef _WIN32
		// Windows-specific conversion using MultiByteToWideChar
		int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		result.resize(requiredSize);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], requiredSize);
		return result;
#elif defined(__EMSCRIPTEN__)
		// WASM-specific conversion (if needed, usually UTF-8 already works)
		return std::wstring(str.begin(), str.end());
#else
		icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(str);
		result.resize(ustr.countChar32());

		for (int32_t i = 0, j = 0; i < ustr.countChar32(); ++i)
		{
			result[i] = ustr.char32At(i);
		}
		return result;
#endif
	}
	std::wstring conversions::from_utf8_to_utf16(const std::u8string& str) {
		std::wstring result;

#ifdef _WIN32
		// Windows-specific conversion using MultiByteToWideChar
		int requiredSize = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(str.c_str()), -1, nullptr, 0);
		result.resize(requiredSize);
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(str.c_str()), -1, &result[0], requiredSize);
#elif defined(__EMSCRIPTEN__)
		// WASM-specific conversion (if needed, usually UTF-8 already works)
		// Example: WASM usually works well with UTF-8 encoded strings and can directly handle them
		result = std::wstring(str.begin(), str.end());
#else
		// Linux/POSIX conversion using mbstowcs
		size_t len = str.size();
		result.resize(len);
		mbstowcs(&result[0], reinterpret_cast<const char*>(str.c_str()), len);
#endif

		return result;
	}

	std::string conversions::from_utf16_to_utf8(const std::wstring& str) {
		std::string result;

#ifdef _WIN32
		// Windows-specific conversion using WideCharToMultiByte
		int requiredSize = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
		result.resize(requiredSize);
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &result[0], requiredSize, nullptr, nullptr);
#elif defined(__EMSCRIPTEN__)
		// WASM-specific conversion (if needed, usually UTF-16 to UTF-8 works natively)
		result = std::string(str.begin(), str.end());
#else
		// Linux/POSIX conversion using wcstombs
		size_t len = str.size();
		result.resize(len * 4);  // To accommodate worst-case multibyte size
		size_t converted = wcstombs(&result[0], str.c_str(), result.size());
		result.resize(converted);  // Adjust to the actual length
#endif	

		return result;
	}

	std::u8string conversions::from_utf16_to_utf8_u8_string(const std::wstring& str) {
		std::u8string result;

#ifdef _WIN32
		// Windows-specific conversion using WideCharToMultiByte
		int requiredSize = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
		result.resize(requiredSize);
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, reinterpret_cast<char*>(&result[0]), requiredSize, nullptr, nullptr);

		return result;

#elif defined(__EMSCRIPTEN__)
		// WASM-specific conversion (if needed, usually UTF-16 to UTF-8 works natively)
		result = std::string(str.begin(), str.end());
#else
		if (str.empty())
        return {};

		std::mbstate_t state = {};
		const wchar_t* src = str.c_str();	

		// Get length needed
		size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
		if (len == static_cast<size_t>(-1))
			return {};  // conversion failed

		std::vector<char> buf(len);
		src = str.c_str();
		std::wcsrtombs(buf.data(), &src, len, &state);

		return std::u8string(reinterpret_cast<char8_t*>(buf.data()), buf.size());

#endif
	}
}
