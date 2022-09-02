/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

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

// Windows
#define D3D_DEBUG_INFO
#include <windowsx.h>
#include <d2d1.h>
//#include <d3d10_1.h>
//#include <d3d10.h>
#include <dwrite.h>
#include <wincodec.h>

// Custom

#include <imgui.h>
#include <imgui_impl_dx10.h>
#include <imgui_impl_win32.h>

// https://docs.microsoft.com/en-us/windows/win32/medfound/saferelease
template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

// Conversions

