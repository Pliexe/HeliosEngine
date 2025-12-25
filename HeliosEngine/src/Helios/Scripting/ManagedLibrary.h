#pragma once
#include "Helios/Utils/DynamicLibrary.h"
#include <filesystem>
#include <vector>
#include <string>

namespace Helios
{
    class ManagedLibrary
    {
    public:
        bool Load(const std::filesystem::path &path);
        void Unload();
        void *GetMethod(const std::string &method_name);

        template <typename ReturnType, typename... Args>
        auto GetMethod(const std::string &method_name)
        {
            return reinterpret_cast<ReturnType (*)(Args...)>(GetMethod(method_name));
        }

    private:
        Unique<DynamicLibrary> m_library;
        void* m_runtime_host = nullptr; // Add this to store the runtime handle
        void *m_load_assembly_and_get_function_pointer = nullptr;
        void *m_another_function_pointer = nullptr;

        bool LoadHostFxr(const std::filesystem::path &path);

        static std::filesystem::path get_hostfxr_path();
    };
}
