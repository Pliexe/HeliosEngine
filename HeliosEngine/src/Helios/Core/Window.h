#pragma once
#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios
{
	class HELIOS_API Window
	{
	public:
		struct Specifications
		{
			std::string title;
			uint32_t width;
			uint32_t height;
		};

		virtual void Show(bool fullscreen) = 0;
		virtual void Hide() = 0;
		virtual bool IsFocused() = 0;

		virtual bool Create(Specifications specs) = 0;
	};
}