#pragma once

#include "pch.h"
#include <string>

#define LOG_ERROR(str) printf("ERROR : %s\n", str);

#define LOG_WARNING(str) printf("WARNING : %s\n", str);

#ifdef HELIOS_PLATFORM_WINDOWS
	#define HL_MESSAGE(message) ::MessageBoxA(NULL, message, "Critical Error!", MB_ICONERROR);
#elif defined(HELIOS_PLATFORM_LINUX)

	inline void show_error(const char* message) {
		static int has_kdialog = []() {
			return system("which kdialog > /dev/null 2>&1") == 0;
		}();

		static int has_zenity = []() {
			return system("which zenity > /dev/null 2>&1") == 0;
		}();

		if (has_kdialog) {
			std::string str = (std::string("kdialog --error \"") + message + "\"");
			system(str.c_str());
		} else if (has_zenity) {
			std::string str = (std::string("zenity --error --text=\"") + message + "\"");
			system(str.c_str());
		} else {
			std::cerr << "Critical Error: " << message << std::endl;
		}
	}

	#define HL_MESSAGE(message) show_error(message)

#else
	#define HL_MESSAGE(message) std::cout << message << std::endl;
#endif

#ifdef HELIOS_PLATFORM_WINDOWS
inline std::string GetLastErrorAsString(HRESULT hr)
{
	LPSTR messageBuffer = nullptr;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	LocalFree(messageBuffer);

	return message;
}

inline std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}
#endif