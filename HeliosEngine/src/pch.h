/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */

#ifndef PCH_H
#define PCH_H



#include <stddef.h>
#include <cstddef>


#ifndef HELIOS_PLATFORM_WINDOWS
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__) || defined(__CYGWIN32__) || defined(__CYGWIN64__) || defined(_WIN32) || defined(__WIN32__)
#define HELIOS_PLATFORM_WINDOWS
#endif
#endif 


#if (defined(_M_IX86) || defined(_M_X64) || __i386__ || __x86_64__) && !defined(M_HYBRID_X86_ARM64) && !defined(_M_ARM64EC)
#define __SSE_ENABLED__
#include <xmmintrin.h>
#endif

#define ENTT_ID_TYPE std::uint64_t

#ifdef _DEBUG
#define HL_DEBUG
#endif

// Windows
#ifdef HELIOS_PLATFORM_WINDOWS
#define D3D_DEBUG_INFO

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windowsx.h>
#include <d2d1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <wincodec.h>
#include <commdlg.h>
#include <shellapi.h>

#endif

// POSIX Platform

#if defined(__unix__) || defined(__APPLE__)
#include <dlfcn.h> // For dlopen, dlsym, dlclose....
#endif

// Linux Platform

#ifdef HELIOS_PLATFORM_LINUX

// #include <gtk/gtk.h>

#endif

// Windows Platform

#ifdef HELIOS_PLATFORM_WINDOWS

 #include <wrl.h>
//  #include <corecrt_math_defines.h>

#else

#endif

//#include <char8_t-remediation.h>s

#ifdef _CRT_INTERNAL_CPP
#undef uintptr_t
#endif

#ifdef uintptr_t
#undef uintptr_t
#endif


// Standard Library

#include <locale>
#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <type_traits>
#include <queue>
#include <deque>
#include <future>
#include <mutex>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <memory>
#include <deque>
#include <random>
#include <ranges>
#include <cstdint>
#include <chrono>
#include <span>
#include <cstdlib>
#include <variant>
#include <iterator>

#include <json.hpp>
using json = nlohmann::json;


#ifdef HELIOS_INCLUDE_IMGUI

// ImGUI
#include <imgui.h>

#include <imgui_stdlib.h>

#endif

// Yaml

#include <yaml-cpp/yaml.h>

// stb

// entt
#include <entt.hpp>
// Custom

#include <Helios/Core/Types.h>
#include <Helios/Core/Constants.h>

#if defined(HELIOS_PLATFORM_WINDOWS) | defined(HELIOS_PLATFORM_LINUX)
// https://docs.microsoft.com/en-us/windows/win32/medfound/saferelease
template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}
#endif // HELIOS_PLATFORM_WINDOWS

#endif // PCH_H