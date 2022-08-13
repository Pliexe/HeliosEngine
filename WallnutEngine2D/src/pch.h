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

// Windows

#include <windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

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

// Conversions

