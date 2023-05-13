/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#ifdef _DEBUG
#define HL_DEBUG
#endif

#ifdef HELIOS_BUILD_DLL
#define HELIOS_API __declspec(dllexport)
#else
#define HELIOS_API __declspec(dllimport)
#endif

typedef unsigned long long bitmask_t;
#define BIT(x) (1ull << static_cast<unsigned long long>(x))

#define HL_DEBUGBREAK __debugbreak

#define HL_INIT_APPLICATION(ClassName) Helios::Application* Helios::CreateApplication() { return new ClassName(); }