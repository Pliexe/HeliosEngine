#pragma once

#include <iostream>
#include <Helios/Scene/Scene.h>

namespace Helios {
	namespace Project {
		std::string currentScenePath;

		Scene& AttempToLoadPreviousScene();
	}
}