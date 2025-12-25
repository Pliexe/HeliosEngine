#pragma once

namespace Helios::Scripting
{
    enum class ServiceResult
    {
        Ok = 0,

        // Generic
        BadServiceName = 1,
        ServiceNotFound = 2,
        ServiceAlreadyExists = 3,
        NullPointerPassed = 4,

        // Descriptor
        BadDescriptor = 5,

        // Service 1000
        ServiceAlreadyRunning = 1000,
        ServiceRunning = 1001,
        ServiceNotRunning = 1002,

        StoppedDuringCallback = 1005,

        BehaviourNotFound = 2000,
        BehaviourCreationFailed = 2001,
        BehaviourDestructionFailed = 2002,
        // BehaviourMethodNotFound = 2003,

        // Shared Library - 5000
        SharedLibraryNotFound = 5000,
        SharedLibraryLoadError = 5001,
        SharedLibraryLoadFunctionNotFound = 5002,
    };

    inline std::string to_string(ServiceResult result)
    {
        switch (result)
        {
        case ServiceResult::Ok:
            return "Ok";
        case ServiceResult::BadServiceName:
            return "BadServiceName";
        case ServiceResult::ServiceNotFound:
            return "ServiceNotFound";
        case ServiceResult::ServiceAlreadyExists:
            return "ServiceAlreadyExists";
        case ServiceResult::NullPointerPassed:
            return "NullPointerPassed";

        case ServiceResult::BadDescriptor:
            return "BadDescriptor";

        case ServiceResult::ServiceAlreadyRunning:
            return "ServiceAlreadyRunning";
        case ServiceResult::ServiceRunning:
            return "ServiceRunning";
        case ServiceResult::ServiceNotRunning:
            return "ServiceNotRunning";

        case ServiceResult::StoppedDuringCallback:
            return "StoppedDuringCallback";

        case ServiceResult::BehaviourNotFound:
            return "BehaviourNotFound";
        case ServiceResult::BehaviourCreationFailed:
            return "BehaviourCreationFailed";
        case ServiceResult::BehaviourDestructionFailed:
            return "BehaviourDestructionFailed";

        case ServiceResult::SharedLibraryNotFound:
            return "SharedLibraryNotFound";
        case ServiceResult::SharedLibraryLoadError:
            return "SharedLibraryLoadError";
        case ServiceResult::SharedLibraryLoadFunctionNotFound:
            return "SharedLibraryLoadFunctionNotFound";
        }

        return "Unknown error";
    }
}