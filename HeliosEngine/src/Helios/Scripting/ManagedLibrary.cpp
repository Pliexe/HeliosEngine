#include "ManagedLibrary.h"
#include "Helios/Core/Exceptions.h"
#include <filesystem>
#include <hostfxr.h>

#include "Helios/Core/Application.h"
#include "Helios/Utils/Conversions.h"

namespace Helios
{
	std::vector<std::string> error_messages;

	void error_writer(const char_t* message) {
		if (message) {
#ifdef HELIOS_PLATFORM_WINDOWS
			error_messages.emplace_back(conversions::from_utf16_to_utf8(message));
#else
			error_messages.emplace_back(message);
#endif
		}
	}
	
	struct HostFxrFunctions
	{
		//using hostfxr_initialize_for_runtime_config_fn = int (*)(const char_t*, const hostfxr_initialize_parameters*, hostfxr_handle*);
		//using hostfxr_get_runtime_delegate_fn = int (*)(hostfxr_handle, hostfxr_delegate_type, void**);

		hostfxr_initialize_for_runtime_config_fn initialize_for_runtime_config = nullptr;
		hostfxr_get_runtime_delegate_fn get_runtime_delegate = nullptr;
		hostfxr_set_error_writer_fn set_error_writer = nullptr;
		hostfxr_close_fn close = nullptr;
	};

	inline static HostFxrFunctions s_hostfxr_functions;

	std::filesystem::path ManagedLibrary::get_hostfxr_path() {
#if defined(HELIOS_PLATFORM_WINDOWS)
		std::filesystem::path dotnet_path = L"C:\\Program Files\\dotnet\\host\\fxr\\";
		if (const char* env_p = std::getenv("DOTNET_ROOT")) {
			dotnet_path = std::filesystem::path(env_p) / "host/fxr/";
		}
		else if (const char* path_env = std::getenv("Path")) {
			std::string path_str(path_env);
			size_t pos = 0;
			std::string token;
			while ((pos = path_str.find(';')) != std::string::npos) {
				token = path_str.substr(0, pos);
				if (token.find("dotnet") != std::string::npos) {
					dotnet_path = std::filesystem::path(token) / "host/fxr/";
					break;
				}
				path_str.erase(0, pos + 1);
			}
		}
#elif defined(HELIOS_PLATFORM_LINUX)
		std::filesystem::path dotnet_path = "/usr/share/dotnet/host/fxr/";
#elif defined(HELIOS_PLATFORM_MACOS)
		std::filesystem::path dotnet_path = "/usr/local/share/dotnet/host/fxr/";
#else
#error "Unsupported platform"
#endif

		for (const auto& dir : std::filesystem::directory_iterator(dotnet_path)) {
			if (dir.is_directory()) {
#if defined(HELIOS_PLATFORM_WINDOWS)
				return dir.path() / L"hostfxr.dll";
#elif defined(HELIOS_PLATFORM_LINUX)
				return dir.path() / "libhostfxr.so";
#elif defined(HELIOS_PLATFORM_MACOS)
				return dir.path() / "libhostfxr.dylib";
#else
#error "Unsupported platform"
#endif
			}
		}

		return "";

	}

	bool ManagedLibrary::LoadHostFxr(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			Helios::ShowMessage("File not found!", "Failed to load hostfxr library, the specified managed library is missing!\n" + conversions::from_u8string(path.u8string()));
			return false;
		}

		std::filesystem::path hostfxr_location = get_hostfxr_path();

		m_library = CreateUnique<DynamicLibrary>(hostfxr_location, false);
		if (!m_library)
		{
			HL_EXCEPTION(true, "Failed to load hostfxr library");
			return false;
		}

		s_hostfxr_functions.initialize_for_runtime_config = (hostfxr_initialize_for_runtime_config_fn)m_library->GetSymbol("hostfxr_initialize_for_runtime_config");
		s_hostfxr_functions.get_runtime_delegate = (hostfxr_get_runtime_delegate_fn)m_library->GetSymbol("hostfxr_get_runtime_delegate");
		s_hostfxr_functions.set_error_writer = (hostfxr_set_error_writer_fn)m_library->GetSymbol("hostfxr_set_error_writer");
		s_hostfxr_functions.close = (hostfxr_close_fn)m_library->GetSymbol("hostfxr_close");

		if (!s_hostfxr_functions.initialize_for_runtime_config || !s_hostfxr_functions.get_runtime_delegate || !s_hostfxr_functions.set_error_writer)
		{
			HL_EXCEPTION(true, "Failed to load hostfxr functions");
			return false;
		}

		s_hostfxr_functions.set_error_writer(error_writer);

		hostfxr_initialize_parameters init_params;
		init_params.size = sizeof(hostfxr_initialize_parameters);
		init_params.host_path = hostfxr_location.c_str();
		#if defined(HELIOS_PLATFORM_WINDOWS)
		init_params.dotnet_root = LR"(C:\Program Files\dotnet)";
#elif defined(HELIOS_PLATFORM_LINUX)
		init_params.dotnet_root = "/usr/share/dotnet";
#elif defined(HELIOS_PLATFORM_MACOS)
		init_params.dotnet_root = "/usr/local/share/dotnet";
#else
#error "Unsupported platform"
#endif
		std::filesystem::path parent_path = path.parent_path();
		if (!std::filesystem::exists(parent_path)) {
			HL_EXCEPTION(true, "Parent path does not exist: " + conversions::from_u8string(parent_path.u8string()));
			return false;
		}
		std::filesystem::path runtime_config = parent_path / (path.stem().u8string() + u8".runtimeconfig.json");

        int rc = s_hostfxr_functions.initialize_for_runtime_config(runtime_config.c_str(), &init_params, &m_runtime_host);
        if (rc != 0 || m_runtime_host == nullptr)
        {
	        std::string error_message = "Errors:";
			for (const auto& error : error_messages) error_message += "\n\t" + error;
			std::cout << error_message << std::endl;
	        HL_EXCEPTION(true, "Failed to initialize .NET Core runtime: " + error_message);
	        return false;
        }
		

		void* load_assembly_fn = nullptr;
		rc = s_hostfxr_functions.get_runtime_delegate(m_runtime_host, hdt_load_assembly_and_get_function_pointer, &load_assembly_fn);
		if (rc != 0 || load_assembly_fn == nullptr) {
			std::string error_message = "Errors:";
			for (const auto& error : error_messages) error_message += "\n\t" + error;
			HL_EXCEPTION(true, "Failed to get load assembly delegate: " + error_message);
			return false;
		}

		m_load_assembly_and_get_function_pointer = load_assembly_fn;

		rc = s_hostfxr_functions.get_runtime_delegate(m_runtime_host, hdt_get_function_pointer, &m_another_function_pointer);
		if (rc != 0 || m_another_function_pointer == nullptr) {
			std::string error_message = "Errors:";
			for (const auto& error : error_messages) error_message += "\n\t" + error;
			HL_EXCEPTION(true, "Failed to get function pointer delegate: " + error_message);
			return false;
		}

		using load_assembly_fn_t = int32_t (*)(const char_t* assembly_path, void* reserved);

		auto load_assembly = reinterpret_cast<load_assembly_fn_t>(load_assembly_fn);

		std::filesystem::path helios_script_path = path.parent_path() / "HeliosScript.dll";

		if (!std::filesystem::exists(helios_script_path))
		{
			HL_EXCEPTION(true, "Failed to load assembly: " + conversions::from_u8string(helios_script_path.u8string()) + "\nFile not found!");
		}
		

		#ifdef HELIOS_PLATFORM_WINDOWS
		rc = load_assembly(path.wstring().c_str(), nullptr); // Load the assembly
		#else
		rc = load_assembly(path.string().c_str(), nullptr);
		#endif
		if (rc != 0)
		{
			std::string error_message = "Errors:";
			for (const auto& error : error_messages) error_message += "\n\t" + error;
			HL_EXCEPTION(true, "Failed to load assembly: " + conversions::from_u8string(path.u8string()) + "\n" + error_message);
			return false;
		}

		return true;
	}

	bool ManagedLibrary::Load(const std::filesystem::path& path)
	{
		return LoadHostFxr(path);
	}

	void ManagedLibrary::Unload()
	{
		m_runtime_host = nullptr;
		m_library.reset();
		m_load_assembly_and_get_function_pointer = nullptr;
		m_another_function_pointer = nullptr;
	}

	void* ManagedLibrary::GetMethod(const std::string& method_name)
	{
		void* function_pointer = nullptr;
		int rc = s_hostfxr_functions.get_runtime_delegate(m_runtime_host, hdt_get_function_pointer, &function_pointer);
		if (rc != 0 || function_pointer == nullptr)
		{
			HL_EXCEPTION(true, "Failed to get runtime delegate");
			return nullptr;
		}

		using get_function_fn = int(*)(const char_t* type_name, const char_t* method_name, const char_t* delegate_type_name, void* reserved, void** delegate);
		auto get_function = reinterpret_cast<get_function_fn>(function_pointer);

		void* function = nullptr;
		std::basic_string<char_t> method_name_t(method_name.begin(), method_name.end());
		rc = get_function(nullptr, method_name_t.c_str(), nullptr, nullptr, &function);
		if (rc != 0 || function == nullptr)
		{
			if (rc != 0)
			{
				std::string msg = "Errors:";
				for (const auto& error : error_messages) msg += "\n\t" + error;
				Helios::ShowMessage("Error", "Failed to get method: " + method_name + "\n" + msg);
			}
			return nullptr;
		}

		return function;
	}
}
