#include "ScriptingService.h"
#include "ScriptManager.h"
// #include "Helios/Core/Asserts.h"

using namespace Helios;
using namespace Helios::Scripting;

ScriptingService::ScriptingService(std::string_view name, const Scripting::ServiceDescriptor &desc): m_Name(name), m_Desc(desc)
{
    // Validate descriptor
    if (desc.shared_library_path.empty())
    {
        if (desc.initialize_fn == nullptr) throw Scripting::ServiceResult::BadDescriptor;
        if (desc.shutdown_fn == nullptr) throw Scripting::ServiceResult::BadDescriptor;
        if (desc.create_behaviour_fn == nullptr) throw Scripting::ServiceResult::BadDescriptor;
        if (desc.destroy_behaviour_fn == nullptr) throw Scripting::ServiceResult::BadDescriptor;
        if (desc.call_behaviour_method_fn == nullptr) throw Scripting::ServiceResult::BadDescriptor;
        if (desc.batch_call_behaviour_method_fn == nullptr) throw Scripting::ServiceResult::BadDescriptor;
    }
    else
    {
        if (!std::filesystem::exists(desc.shared_library_path))
        {
            throw Scripting::ServiceResult::SharedLibraryNotFound;
        }

        m_AssemblyPath = desc.shared_library_path;
    }
}

Scripting::ServiceResult ScriptingService::Start()
{
    if (m_Status == Scripting::ServiceStatus::Running) return Scripting::ServiceResult::ServiceAlreadyRunning;

    if (!m_Desc.shared_library_path.empty())
    {
        m_Assembly = CreateUnique<DynamicLibrary>(m_AssemblyPath);

        m_Initialize = (Scripting::ServiceFunctions::Initialize)m_Assembly->GetSymbol("Initialize");
        m_Shutdown = (Scripting::ServiceFunctions::Shutdown)m_Assembly->GetSymbol("Shutdown");
        m_CreateBehaviour = (Scripting::ServiceFunctions::CreateBehaviour)m_Assembly->GetSymbol("CreateBehaviour");
        m_DestroyBehaviour = (Scripting::ServiceFunctions::DestroyBehaviour)m_Assembly->GetSymbol("DestroyBehaviour");
        m_CallBehaviourMethod = (Scripting::ServiceFunctions::CallBehaviourMethod)m_Assembly->GetSymbol("CallBehaviourMethod");
        m_BatchCallBehaviourMethod = (Scripting::ServiceFunctions::BatchCallBehaviourMethod)m_Assembly->GetSymbol("BatchCallBehaviourMethod");

        if (m_Initialize == nullptr || m_Shutdown == nullptr || m_CreateBehaviour == nullptr || m_DestroyBehaviour == nullptr
            || m_CallBehaviourMethod == nullptr || m_BatchCallBehaviourMethod == nullptr)
        {
            m_Assembly.reset();
            return Scripting::ServiceResult::SharedLibraryLoadFunctionNotFound;
        }
    } else {
        m_Initialize = m_Desc.initialize_fn;
        m_Shutdown = m_Desc.shutdown_fn;
        m_CreateBehaviour = m_Desc.create_behaviour_fn;
        m_DestroyBehaviour = m_Desc.destroy_behaviour_fn;
        m_CallBehaviourMethod = m_Desc.call_behaviour_method_fn;
        m_BatchCallBehaviourMethod = m_Desc.batch_call_behaviour_method_fn;
    }

    m_Initialize(this, [](void* service, const char* behaviour_name) {
        if (service == nullptr || behaviour_name == nullptr) return;
        auto service_ptr = static_cast<ScriptingService*>(service);
        if (service_ptr == nullptr) return;
        service_ptr->m_BehaviourNames.push_back(behaviour_name);
    });

    m_Status = Scripting::ServiceStatus::Running;

    return Scripting::ServiceResult::Ok;
}

Scripting::ServiceResult ScriptingService::Stop()
{
    if (m_Status != Scripting::ServiceStatus::Running) return Scripting::ServiceResult::ServiceNotRunning;
    m_Status = Scripting::ServiceStatus::Stopped;

    if (!m_Desc.shared_library_path.empty())
    {
        m_Shutdown();

        m_Initialize = nullptr;
        m_Shutdown = nullptr;
        m_CreateBehaviour = nullptr;
        m_DestroyBehaviour = nullptr;
        m_CallBehaviourMethod = nullptr;
        m_BatchCallBehaviourMethod = nullptr;

        m_Assembly.reset();
    }

    m_BehaviourNames.clear();

    return Scripting::ServiceResult::Ok;
}

Scripting::ServiceResult ScriptingService::CreateBehaviour(std::string_view behaviour_name, EntityPtr entity, BehaviourHandle *out_handle)
{
    assert(entity != 0);
    if (m_Status != Scripting::ServiceStatus::Running) return Scripting::ServiceResult::ServiceNotRunning;
    if (std::find(m_BehaviourNames.begin(), m_BehaviourNames.end(), behaviour_name) == m_BehaviourNames.end()) return Scripting::ServiceResult::BehaviourNotFound;

    auto result = m_CreateBehaviour(behaviour_name.data(), entity);
    if (result == 0) return Scripting::ServiceResult::BehaviourCreationFailed;

    *out_handle = result;

    return Scripting::ServiceResult::Ok;
}

Scripting::ServiceResult ScriptingService::DestroyBehaviour(BehaviourHandle behaviour_handle)
{
    assert(behaviour_handle != 0); // Fail Null? Compiler warning when using NULL.
    if (m_Status != Scripting::ServiceStatus::Running) return Scripting::ServiceResult::ServiceNotRunning;

    auto result = m_DestroyBehaviour(behaviour_handle);

    return result ? Scripting::ServiceResult::Ok : Scripting::ServiceResult::BehaviourDestructionFailed;
}

Scripting::ServiceResult ScriptingService::CallBehaviourMethod(const BehaviourHandle behaviour_handle, std::string_view method_name)
{
    assert(behaviour_handle != 0); // Fail Null? Compiler warning when using NULL.
    if (m_Status != Scripting::ServiceStatus::Running) return Scripting::ServiceResult::ServiceNotRunning;

    m_CallBehaviourMethod(behaviour_handle, method_name.data());

    return Scripting::ServiceResult::Ok;
    // return m_CallBehaviourMethod(behaviour_handle, method_name.data()) ? Scripting::ServiceResult::Ok : Scripting::ServiceResult::BehaviourMethodNotFound;
}

Scripting::ServiceResult ScriptingService::BatchCallBehaviourMethod(const BehaviourHandle *behaviour_handles, uint32_t count, std::string_view method_name)
{
    assert(behaviour_handles != nullptr);
    if (m_Status != Scripting::ServiceStatus::Running) return Scripting::ServiceResult::ServiceNotRunning;

    m_BatchCallBehaviourMethod(behaviour_handles, count, method_name.data());

    return Scripting::ServiceResult::Ok;
}

Scripting::ServiceResult ScriptingService::BatchCallBehaviourMethod(std::initializer_list<BehaviourHandle> behaviour_handles, std::string_view method_name)
{
    return BatchCallBehaviourMethod(behaviour_handles.begin(), behaviour_handles.size(), method_name);
}
