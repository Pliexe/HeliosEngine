#pragma once
#include "pch.h"
#include "BehaviourHandle.h"

namespace Helios::Scripting
{
#ifdef __cplusplus
    typedef ::std::uintptr_t EntityPtr;
#else
    typedef void* EntityPtr;
#endif

    namespace ServiceFunctions
    {
        using InitializeFnRegisterBehaviourCb = void(*)(void* handler, const char* behaviour_name);
        using Initialize = void(*)(void* handler, InitializeFnRegisterBehaviourCb cb);
        using Shutdown = void(*)(void);
        using CreateBehaviour = BehaviourHandle(*)(const char* behaviour_name, EntityPtr entity);
        using DestroyBehaviour = bool(*)(const BehaviourHandle handle);
        using CallBehaviourMethod = void(*)(const BehaviourHandle handle, const char* method_name);
        using BatchCallBehaviourMethod = void(*)(const BehaviourHandle* handles, uint32_t count, const char* method_name);
    }
}