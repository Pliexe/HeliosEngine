/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */

#ifndef PCH_H
#define PCH_H

#if (defined(_M_IX86) || defined(_M_X64) || __i386__ || __x86_64__) && !defined(M_HYBRID_X86_ARM64) && !defined(_M_ARM64EC)
#define __SSE_ENABLED__
#include <xmmintrin.h>
#endif

#define ENTT_ID_TYPE std::uint64_t

#ifdef _DEBUG
#define HL_DEBUG
#endif

#define PI 3.14159265358979323846f

// Standard Library

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
#include <chrono>

// Windows
#define D3D_DEBUG_INFO

#ifdef HELIOS_PLATFORM_WINDOWS

#include <windowsx.h>
#include <d2d1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <wincodec.h>
#define NOMINMAX

#endif

#ifdef HELIOS_PLATFORM_LINUX

#include <gtk/gtk.h>

#endif

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <json.hpp>
using json = nlohmann::json;

#ifdef HELIOS_PLATFORM_WINDOWS

 #include <wrl.h>
//  #include <corecrt_math_defines.h>

#else

#endif

#define H_E        2.71828182845904523536   // e
#define H_LOG2E    1.44269504088896340736   // log2(e)
#define H_LOG10E   0.434294481903251827651  // log10(e)
#define H_LN2      0.693147180559945309417  // ln(2)
#define H_LN10     2.30258509299404568402   // ln(10)
#define H_PI       3.14159265358979323846   // pi
#define H_PI_2     1.57079632679489661923   // pi/2
#define H_PI_4     0.785398163397448309616  // pi/4
#define H_1_PI     0.318309886183790671538  // 1/pi
#define H_2_PI     0.636619772367581343076  // 2/pi
#define H_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define H_SQRT2    1.41421356237309504880   // sqrt(2)
#define H_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)


#ifdef HELIOS_INCLUDE_IMGUI

// ImGUI
#include <imgui.h>

#include <imgui_stdlib.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#endif

// Yaml

#include <yaml-cpp/yaml.h>

// stb

// entt
#include <entt.hpp>
// Custom

// https://docs.microsoft.com/en-us/windows/win32/medfound/saferelease
template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}


namespace Helios {

    template <typename T>
    using Scope = std::unique_ptr<T>;
    template <typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args) { return std::make_unique<T>(std::forward<Args>(args)...); }

    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) { return std::make_shared<T>(std::forward<Args>(args)...); }

    template <typename T>
    using WeakRef = std::weak_ptr<T>;
}

#endif