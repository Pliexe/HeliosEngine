/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Application.h"

extern Wallnut::Application* Wallnut::CreateApplication();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance /*hahaha 16bit windows legacy*/, PWSTR pCmdLine, int nCmdShow)
{
	auto app = Wallnut::CreateApplication();
	int exitCode = app->Run();
	if (SUCCEEDED(exitCode)) app->Init();
	delete app;
	return exitCode;
}