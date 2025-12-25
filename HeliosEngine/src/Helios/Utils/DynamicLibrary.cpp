#include "DynamicLibrary.h"

#include "Conversions.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Core/Logger.h"

namespace Helios
{
	DynamicLibrary::DynamicLibrary(const std::filesystem::path &path, bool format)
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		m_Handle = LoadLibraryW((format ? (path.wstring() + L".dll") : path.wstring()).c_str());
#elif defined(HELIOS_PLATFORM_LINUX)
		m_Handle = dlopen((format ? (path.string() + ".so") : path.string()).c_str(), RTLD_LAZY);
#endif
		std::cout << "-------------------->" << "LOADED SHARED LIBRARY: " << conversions::from_u8string(path.generic_u8string()) << "\n";
#ifdef HELIOS_PLATFORM_WINDOWS
		HL_EXCEPTION_HR(m_Handle == nullptr, "Failed to load dynamic library: " + conversions::from_u8string(path.generic_u8string()), GetLastError());
#else
		HL_EXCEPTION(m_Handle == nullptr, "Failed to load dynamic library: " + conversions::from_u8string(path.generic_u8string()));
#endif
	}

#ifdef HELIOS_PLATFORM_WINDOWS
	DWORD WINAPI UnloadThread(LPVOID param)
	{
		HMODULE hModule = (HMODULE)param;
		FreeLibraryAndExitThread(hModule, 0);
		return 0;
	}
#endif

	DynamicLibrary::~DynamicLibrary()
	{
		std::cout << "-------------------->" << "TRYING UNLOADING SHARED LIBRARY" << "\n";
#ifdef HELIOS_PLATFORM_WINDOWS

		HMODULE hModule = nullptr;
		if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, L"UserScript.dll", &hModule))
		{
			std::wcout << L"UserScript.dll has multiple references.\n";
		}

		// if (FreeLibrary((HMODULE)m_Handle) == 0)
		// {
		// 	HL_EXCEPTION_HR(true, "Failed to unload dynamic library", GetLastError());
		// }

		HANDLE hThread = CreateThread(NULL, 0, UnloadThread, (LPVOID)m_Handle, 0, NULL);
        if (hThread)
        {
            CloseHandle(hThread);
        }

		hModule = nullptr;
		if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"UserScript.dll", &hModule))
		{
			std::wcout << L"UserScript.dll is still loaded.\n";
		}
		else
		{
			std::wcout << L"UserScript.dll is not loaded.\n";
		}

#elif defined(HELIOS_PLATFORM_LINUX)
		if (dlclose(m_Handle) != 0)
		{
		#ifdef HELIOS_DEBUG
			std::string str = "Failed to unload dynamic library: " + std::string(dlerror());
			HL_MESSAGE(str.c_str());
		#else
			std::cout << "Failed to unload dynamic library: " + std::string(dlerror()) << std::endl;
		#endif
		}
#endif
		std::cout << "-------------------->" << "DONE UNLOADING SHARED LIBRARY" << "\n";
	}
	void *DynamicLibrary::GetSymbol(const std::string &name)
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		return GetProcAddress((HMODULE)m_Handle, name.c_str());
#elif defined(HELIOS_PLATFORM_LINUX)
		return dlsym(m_Handle, name.c_str());
#endif
		std::cout << "-------------------->" << "LOADED SYMBOL: " << name << "\n";
	}
}