#pragma once
#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios
{
	class HELIOS_API DynamicLibrary
	{
	public:
		DynamicLibrary(const std::filesystem::path& path);
		~DynamicLibrary();

		void* GetFunction(const std::string& name);

	private:
		void* m_Handle;
	};
}