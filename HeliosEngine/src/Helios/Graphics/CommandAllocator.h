#pragma once

#include "pch.h"

#include "NativeCommandList.h"

namespace Helios
{
    class CommandAllocator
    {
    public:
        CommandAllocator() = default;
        virtual ~CommandAllocator() = default;

        virtual Ref<NativeCommandList> CreateCommandList() = 0;
        virtual Ref<NativeCommandListImmediate> CreateCommandListImmediate() = 0;

        // virtual Ref<NativeCommandList> CreateTransientCommandList() = 0;
        // virtual Ref<NativeCommandListImmediate> CreateTransientCommandListImmediate() = 0;

        virtual void Reset() = 0;

        // CreateCommandList&
    
#pragma region Disable copy and move
        CommandAllocator(const CommandAllocator&) = delete;
        CommandAllocator& operator=(const CommandAllocator&) = delete;

        CommandAllocator(CommandAllocator&&) = delete;
        CommandAllocator& operator=(CommandAllocator&&) = delete;
#pragma endregion // Disable copy and move
        
#pragma region Allocations of CommandAllocator per thread

        static CommandAllocator& GetForCurrentThread();

#pragma endregion // Allocations of CommandAllocator per thread

    private:
        // friend class Application;

    };
}