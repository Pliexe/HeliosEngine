/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Application.h"

extern Helios::Application* Helios::CreateApplication();
#ifdef HELIOS_EDITOR
extern int ValidateInit();
#endif // HELIOS_EDITOR

#include "Asserts.h"

#include "Helios/Translation/Matrix.h"
#include "Helios/Translation/Vector.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance /*hahaha 16bit windows legacy*/, PWSTR pCmdLine, int nCmdShow)
{

#pragma region Logging

	bool consoleEnabled = false;

#ifndef _DEBUG
	if (__argc > 1) {
		for (int i = 0; i < __argc; i++)
			if (strcmp(__argv[i], "-debug") || strcmp(__argv[i], "-console") || strcmp(__argv[i], "-logs"))
			{
#endif
				consoleEnabled = true;

				AllocConsole();

				SetConsoleTitle(L"Debug Console");

				freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
				freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

#ifndef _DEBUG
			}
	}
#endif

	if (!consoleEnabled) {

		FILE* o;
		FILE* e;

		freopen_s(&o, "logs.txt", "w", stdout);
		freopen_s(&e, "error.txt", "w", stderr);

	}

	std::cout << "Starting Application" << std::endl;

#pragma endregion

	int result;
	
#ifdef HELIOS_EDITOR
	result = ValidateInit();
	if (result < 0) return result;
#endif // HELIOS_EDITOR

	try {
		auto app = Helios::CreateApplication();
		result = app->Run();
		delete app;
	} catch(Helios::HeliosException e) {
		e.what(false);
		return -1;
	}
	std::cout << "Application closed! Code: " << result << std::endl;
	return result;
}