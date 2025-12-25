#pragma once
#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios
{
	class HELIOS_API DynamicLibrary
	{
	public:
		DynamicLibrary(const std::filesystem::path& path, bool format = false);
		~DynamicLibrary();

		void* GetSymbol(const std::string& name);

		template <typename ReturnType, typename... Args>
		auto GetSymbol(const std::string& name)
		{
#ifdef HELIOS_PLATFORM_WINDOWS
			return static_cast<ReturnType(*)(Args...)>(GetSymbol(name));
#else // POSIX
			void* sym = GetSymbol(name);

			union {
				void* p;
				ReturnType(*fp)(Args...);
			} caster;

			caster.p = sym;
			return caster.fp;
#endif

		}

	private:
		void* m_Handle;
	};

	template <typename ReturnType, typename... Args>
	using DynamicLibrarySymbol = ReturnType (*)(Args...);
}