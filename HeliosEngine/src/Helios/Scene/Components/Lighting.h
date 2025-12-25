#pragma once
#include "Helios/Resources/Color.h"

namespace Helios
{
	struct HELIOS_API BaseLightComponent
	{
		Color color = Color::White;
		float intensity = 1.0f;

		// Ref<Framebuffer> framebuffer;

		BaseLightComponent() = default;
		BaseLightComponent(const Color& color, float intensity) : color(color), intensity(intensity) {}
	};

	struct HELIOS_API DirectionalLightComponent : public BaseLightComponent
	{
		DirectionalLightComponent() = default;
		DirectionalLightComponent(const Color& color, float intensity) : BaseLightComponent(color, intensity) {}
	};
}
