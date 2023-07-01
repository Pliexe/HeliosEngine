#pragma once
#include "Helios/Resources/Color.h"

namespace Helios
{
	struct HELIOS_API DirectionalLightComponent
	{
		Color color = Color::White;
		float intensity = 1.0f;

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const Color& color, float intensity) : color(color), intensity(intensity) {}
	};
}
