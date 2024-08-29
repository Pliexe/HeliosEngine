#include "DynamicLibrary.h"
#include "Helios/Core/Exceptions.h"

namespace Helios
{
	DynamicLibrary::DynamicLibrary(const std::filesystem::path& path)
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		m_Handle = LoadLibraryW(path.wstring().c_str());
#elif defined(HELIOS_PLATFORM_LINUX)
		m_Handle = dlopen(path.string().c_str(), RTLD_LAZY);
#endif
		
		HL_EXCEPTION(m_Handle == nullptr, "Failed to load dynamic library: " + path.generic_string());
	}
	DynamicLibrary::~DynamicLibrary()
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		FreeLibrary((HMODULE)m_Handle);
#elif defined(HELIOS_PLATFORM_LINUX)
		dlclose(m_Handle);
#endif
	}
	void* DynamicLibrary::GetFunction(const std::string& name)
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		return GetProcAddress((HMODULE)m_Handle, name.c_str());
#elif defined(HELIOS_PLATFORM_LINUX)
		return dlsym(m_Handle, name.c_str());
#endif
	}
}