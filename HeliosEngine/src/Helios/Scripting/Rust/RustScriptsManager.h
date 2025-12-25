#pragma once
#include "pch.h"
#include "Helios/Utils/DynamicLibrary.h"
#include "Helios/Core/Base.h"

namespace Helios
{
	class RustScriptsManager
	{
	public:
		RustScriptsManager(const std::filesystem::path& rust_library_path) : m_rustLibrary(rust_library_path), m_rust_library_path(rust_library_path) {

		}
		//~RustScriptsManager();

		bool Init() {
			auto update_all_scripts = m_rustLibrary.GetSymbol<void>("helios_update_all");

			if (!update_all_scripts) {
				std::cout << "Failed to get update_all_scripts from Rust library\n";
				return false;
			}

			auto awake_all_scripts = m_rustLibrary.GetSymbol<void>("helios_awake_all");

			if (!awake_all_scripts) {
				std::cout << "Failed to get awake_all_scripts from Rust library\n";
				return false;
			}

			std::cout << "Rust Library Path: " << m_rust_library_path << "\n";

			auto funs = m_rustLibrary.GetSymbol<const char* const*>("get_api_structs");

			if (funs) {
				std::cout << "Funs:\n";
				auto ptr = funs();
				while (*ptr != nullptr) {
					std::cout << *ptr << "\n";
					ptr++;
				}
			}
			else std::cout << "Failed to get funs from Rust library\n";

			return true;
		}

		void Shutdown() {

		}

		void OnAwake() {
			m_awake_all_scripts();
		}

		void OnUpdate() {
			m_update_all_scripts();
		}

	private:

		DynamicLibrary m_rustLibrary;
		std::filesystem::path m_rust_library_path;
		std::function<void()> m_update_all_scripts;
		std::function<void()> m_awake_all_scripts;
	};
}
