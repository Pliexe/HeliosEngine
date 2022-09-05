#pragma once

#include <Helios/ExitCodes.h>
#include <iostream>

// VERSION NUMBER

#define STR(str) #str
#define STRING(str) STR(str)

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0
#define VERSION_FULL		STRING(VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH) 
//#define VERSION_FULL_NUMER  VERSION_MAJOR\VERSION_MINOR\VERSION_PATCH 

// DEFAULT CONFIG
struct StartupConfig {
	int window_width = 800;
	int window_height = 600;

	bool is_fullscreen = false;
	std::string game_name = std::string("Game Name");
};


// EXIT CODES
// ERROR -> FROM -5000 and below

#define HELIOS_EXIT_CODE_INVALID_ARGS -5000
#define HELIOS_EXIT_CODE_INVALID_PROJECT_PATH -5001
#define HELIOS_EXIT_CODE_INVALID_PROJECT -5002
