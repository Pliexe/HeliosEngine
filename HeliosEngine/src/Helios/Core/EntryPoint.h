/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#pragma once

#include <cstdlib>
#ifdef HELIOS_PLATFORM_WINDOWS
#include <io.h>
#include <corecrt_io.h>
#endif

#include "Application.h"

extern Helios::Application* Helios::CreateApplication(int argc, char** argv);

#include "Asserts.h"

#include "Helios/Utils/ShowMessage.h"

#include "Helios/Math/Matrix.h"
#include "Helios/Math/Vector.h"

#ifdef USE_QT
void initQt(int argc, char** argv);
#endif

int main(int argc, char** argv)
{

#ifdef USE_QT
	initQt(argc, argv);
#endif
	
#pragma region Logging

	bool consoleEnabled = false;

#ifndef _DEBUG
	if (argc > 1) {
		for (int i = 0; i < argc; ++i) {
			std::string_view arg = argv[i];
			if (arg == "--debug" || arg == "--console" || arg == "--logs") {
				consoleEnabled = true;
				break;
			}
		}
	}
#else
	consoleEnabled = true; // Always enable console in debug mode
#endif

	if (consoleEnabled) {
#ifdef HELIOS_PLATFORM_WINDOWS
		AllocConsole();
		SetConsoleTitleW(L"Debug Console");

		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

		// Optional: Enable UTF-8 output - Disabled due to freezing sometimes probabbly because of line ending
		// SetConsoleOutputCP(CP_UTF8);
		// setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif // HELIOS_PLATFORM_WINDOWS - Otherwise just let it go to console on other platforms
	} else {
#ifdef HELIOS_PLATFORM_WINDOWS
		FILE* o;
		FILE* e;
		freopen_s(&o, "logs.txt", "w", stdout);
		freopen_s(&e, "error.txt", "w", stderr);
#else
		freopen("logs.txt", "w", stdout);
		freopen("error.txt", "w", stderr);
#endif
	}
	
#pragma endregion

	try {
		auto app = Helios::CreateApplication(argc, argv);
		if (app == nullptr) {
			Helios::ShowMessage("An Critical Error Occured!", "Failed to create application!", Helios::Message::IconError | Helios::Message::Flags::Ok);
			return -1;
		}
		app->Run();
		delete app;
	} catch(Helios::HeliosException& e) {
		Helios::ShowMessage("An Critical Error Occured!", e.what(), Helios::Message::IconError | Helios::Message::Flags::Ok);
		//Helios::Helios::ShowMessage("An Critical Error Occured!", e.what(), Helios::Message::IconError | Helios::Message::Flags::Ok);

		abort();
		return -1;
	}

	return 0;
}
