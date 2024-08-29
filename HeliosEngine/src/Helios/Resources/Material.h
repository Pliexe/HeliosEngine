#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Core/Logger.h"
#include "Color.h"
#include "Texture.h"

namespace Helios
{
	class HELIOS_API Material
	{
	public:

		Color Color;
		Ref<Texture> texture;

	public:

		enum class Type
		{
			Warp,
			Mirror,
			Clamp,
			Border,
			MirrorOnce
		};

		enum class Filter
		{
			MinMagMipPoint,
			ComparisonMinMagMipLinear,
		};

		virtual void Bind(uint32_t slot) = 0;
		virtual void Unbind() = 0;

		static Ref<Material> Create(Filter filter, Type type);
	};
}