/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#ifdef _DEBUG
#define HL_DEBUG
#endif

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

// Windows
#define D3D_DEBUG_INFO
#include <windowsx.h>
#include <d2d1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <wincodec.h>

#include <wrl.h>
#include <corecrt_math_defines.h>

#ifdef HELIOS_EDITOR

// ImGUI

#include <imgui.h>

#include <imgui_stdlib.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_dx10.h>
#include <imgui_impl_win32.h>

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