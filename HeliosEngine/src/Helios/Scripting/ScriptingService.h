#pragma once

#include "Helios/Utils/DynamicLibrary.h"
#include "ServiceResult.h"
#include "ServiceFunctions.h"

namespace Helios::Scripting
{
    enum class ServiceStatus : uint8_t
    {
        NotInitialized = 0u,
        Running = 1u,
        Stopped = 2u,
        NotFound = 255u
    };

    static constexpr const char *to_string(ServiceStatus status)
    {
        switch (status)
        {
        case ServiceStatus::NotInitialized:
            return "NotInitialized";
        case ServiceStatus::Running:
            return "Running";
        case ServiceStatus::Stopped:
            return "Stopped";
        case ServiceStatus::NotFound:
            return "NotFound";
        default:
            return "Unknown";
        }
    }

    struct ServiceDescriptor
    {
        // For Static
        ServiceFunctions::Initialize initialize_fn;
        ServiceFunctions::Shutdown shutdown_fn;
        ServiceFunctions::CreateBehaviour create_behaviour_fn;
        ServiceFunctions::DestroyBehaviour destroy_behaviour_fn;
        ServiceFunctions::CallBehaviourMethod call_behaviour_method_fn;
        ServiceFunctions::BatchCallBehaviourMethod batch_call_behaviour_method_fn;
        // For Shared
        std::filesystem::path shared_library_path;
    };

    class ScriptingService
    {
    public:
        ScriptingService() = delete;
        ScriptingService(std::string_view name, const Scripting::ServiceDescriptor &desc);

        const std::string &GetName() const { return m_Name; }
        Scripting::ServiceStatus GetStatus() const { return m_Status; }
        const std::vector<std::string> &GetBehaviourNames() const { return m_BehaviourNames; }

        Scripting::ServiceResult Start();
        Scripting::ServiceResult Stop();

        Scripting::ServiceResult CreateBehaviour(std::string_view behaviour_name, EntityPtr entity, BehaviourHandle *out_handle);
        Scripting::ServiceResult DestroyBehaviour(BehaviourHandle behaviour_handle);

        Scripting::ServiceResult CallBehaviourMethod(const BehaviourHandle behaviour_handle, std::string_view method_name);
        Scripting::ServiceResult BatchCallBehaviourMethod(const BehaviourHandle *behaviour_handles, uint32_t count, std::string_view method_name);

        Scripting::ServiceResult BatchCallBehaviourMethod(std::initializer_list<BehaviourHandle> behaviour_handles, std::string_view method_name);

    private:
        std::string m_Name;
        Scripting::ServiceDescriptor m_Desc;

        std::filesystem::path m_AssemblyPath;
        Unique<DynamicLibrary> m_Assembly;

        std::vector<std::string> m_BehaviourNames;

        Scripting::ServiceStatus m_Status = Scripting::ServiceStatus::NotInitialized;

        Scripting::ServiceFunctions::Initialize m_Initialize;
        Scripting::ServiceFunctions::Shutdown m_Shutdown;
        Scripting::ServiceFunctions::CreateBehaviour m_CreateBehaviour;
        Scripting::ServiceFunctions::DestroyBehaviour m_DestroyBehaviour;
        Scripting::ServiceFunctions::CallBehaviourMethod m_CallBehaviourMethod;
        Scripting::ServiceFunctions::BatchCallBehaviourMethod m_BatchCallBehaviourMethod;
    };
}