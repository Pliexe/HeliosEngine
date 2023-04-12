#pragma once
#include "Helios/Resources/Color.h"

namespace Helios
{
	struct HELIOS_API DirectionalLightComponent
	{
		Color color;
		float intensity;

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const Color& color, float intensity) : color(color), intensity(intensity) {}
	};
}
