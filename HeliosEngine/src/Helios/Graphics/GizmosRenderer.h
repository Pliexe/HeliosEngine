#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Translation/Matrix.h"

namespace Helios
{
	class HELIOS_API GizmosRenderer
	{
	private:

	public:
		
		static void Begin(Matrix4x4 projection);
		static void End();

		
	};
}