/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Application.h"

extern Helios::Application* Helios::CreateApplication(int argc, char** argv);

#include "Asserts.h"

#include "Helios/Utils/ShowMessage.h"

#include "Helios/Math/Matrix.h"
#include "Helios/Math/Vector.h"

int main(int argc, char** argv)
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
	
#pragma endregion

	try {
		auto app = Helios::CreateApplication(argc, argv);
		app->Run();
		delete app;
	} catch(Helios::HeliosException e) {
		Helios::ShowMessage("An Critical Error Occured!", e.what(), Helios::Message::IconError | Helios::Message::Flags::Ok);
		//Helios::Helios::ShowMessage("An Critical Error Occured!", e.what(), Helios::Message::IconError | Helios::Message::Flags::Ok);
		return -1;
	}

	return 0;
}