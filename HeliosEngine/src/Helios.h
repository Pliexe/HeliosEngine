/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Helios/Core/EntryPoint.h"
#include "Helios/Core/Application.h"

#include "Helios/Core/Time.h"

// Initialize your extended application class
#define HL_INIT(ClassName) Helios::Application<ClassName>* Helios::CreateApplication() { return new ClassName(); }