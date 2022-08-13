/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#ifdef WALLNUT_BUILD_DLL
#define WALLNUT_API __declspec(dllexport)
#else
#define WALLNUT_API __declspec(dllimport)
#endif