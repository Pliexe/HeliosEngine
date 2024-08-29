#pragma once
#include "pch.h"
#include "Helios/Utils/DynamicLibrary.h"
#include "Helios/Core/Base.h"

extern "C" {
	void HELIOS_API test_rust();
}

void test_rust()
{
	std::cout << "Hello from C++!\n";
}

namespace Helios
{
	struct MyStruct {
		char** items;
		size_t count;
	};

	extern "C" {
		MyStruct* create_my_struct(const std::vector<std::string>& items);
		void destroy_my_struct(MyStruct* my_struct);
	}

	std::vector<std::string> getApiFunctions(const std::filesystem::path& rust_library_path) {
		std::ifstream file("function_names.bin", std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char> buffer(size + 1);
		if (!file.read(buffer.data(), size)) {
			throw std::runtime_error("Failed to read file");
		}
		buffer[size] = '\0'; // Ensure null-termination

		std::vector<std::string> strings;
		std::string currentString;
		for (char& b : buffer) {
			if (b == '\0') { // Null terminator indicates the end of a string
				strings.push_back(currentString);
				currentString.clear(); // Reset the current string
			}
			else {
				currentString.push_back(b);
			}
		}
		// Add the last string if it wasn't terminated by a null byte
		if (!currentString.empty()) {
			strings.push_back(currentString);
		}

		return strings;
	}


	class RustScriptsManager
	{
	public:
		RustScriptsManager(const std::filesystem::path& rust_library_path) : m_rustLibrary(rust_library_path) { 
			std::cout << "Rust Library Path: " << rust_library_path << "\n";

			void* (*helios_func_my_testfunction)() = reinterpret_cast<void*(*)()>(m_rustLibrary.GetFunction("helios_func_my_testfunction"));

			if (helios_func_my_testfunction) {
				helios_func_my_testfunction();
			}
			else {
				std::cout << "Failed to get helios_func_my_testfunction from Rust library\n";
			}

			void* (*hello_rust)() = reinterpret_cast<void* (*)()>(m_rustLibrary.GetFunction("hello_rust"));

			if (hello_rust) {
				hello_rust();
			}
			else {
				std::cout << "Failed to get hello_rust from Rust library\n";
			}

			MyStruct* (*get_api_functions)(const char*) = reinterpret_cast<MyStruct*(*)(const char*)>(m_rustLibrary.GetFunction("get_api_functions"));

			if (get_api_functions) {
				MyStruct* my_struct = get_api_functions((rust_library_path.parent_path() / "../../function_names.bin").generic_string().c_str());
				for (size_t i = 0; i < my_struct->count; i++) {
					std::cout << my_struct->items[i] << "\n";
				}
				//destroy_my_struct(my_struct);
			}
			else {
				std::cout << "Failed to get API functions from Rust library\n";
			}
		}
		//~RustScriptsManager();

	private:

		//Scope<DynamicLibrary> m_rustLibrary;
		DynamicLibrary m_rustLibrary;
	};
}