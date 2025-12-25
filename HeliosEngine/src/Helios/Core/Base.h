/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#pragma once

#define HELIOS_FILE_SIGNATURE "HELIOS_FILE"

#pragma region Engine Version

#define HELIOS_VERSION_TO_NUMBER(major, minor, patch) \
    (((uint64_t)(major) << 32) | ((uint64_t)(minor) << 16) | (uint64_t)(patch))

#define HELIOS_ENGINE_VERSION HELIOS_VERSION_TO_NUMBER(0, 1, 0)
#define HELIOS_ENGINE_VERSION_STRING "0.1.0"

#pragma endregion // ! Engine Version


#ifdef _DEBUG
#define HL_DEBUG
#endif

#ifdef HELIOS_PLATFORM_WINDOWS
	#ifdef HELIOS_BUILD_DLL
		#define HELIOS_EXPORT __declspec(dllexport)
		#define HELIOS_IMPORT __declspec(dllimport)
	#else
		#define HELIOS_EXPORT
		#define HELIOS_IMPORT
	#endif
#else
	#ifdef HELIOS_BUILD_DLL
		#define HELIOS_EXPORT __attribute__((visibility("default")))
	#else
		#define HELIOS_EXPORT
	#endif
	#define HELIOS_IMPORT
#endif

#ifdef HELIOS_PLATFORM_WINDOWS
	#ifdef HELIOS_BUILD_DLL
		#ifdef HELIOS_EXPORT_DLL
		#define HELIOS_API HELIOS_EXPORT
		#else
		#define HELIOS_API HELIOS_IMPORT
		#endif
	#else
	#define HELIOS_API
	#endif
#else
	#ifdef HELIOS_BUILD_DLL
		#define HELIOS_API HELIOS_EXPORT
	#else
		#define HELIOS_API
	#endif
#endif

typedef unsigned long long bitmask_t;
#define BIT(x) (1ull << static_cast<unsigned long long>(x))

#ifdef _MSC_VER
    #define HL_DEBUGBREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #if __has_builtin(__builtin_debugtrap)
        #define HL_DEBUGBREAK() __builtin_debugtrap()
    #else
        #define HL_DEBUGBREAK() __builtin_trap()
    #endif
#else
    #include <csignal>
    #define HL_DEBUGBREAK() std::raise(SIGTRAP)
#endif

#define HL_INIT_APPLICATION(ClassName) Helios::Application* Helios::CreateApplication() { return new ClassName(); }