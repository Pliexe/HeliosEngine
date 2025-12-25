#pragma once

#include "pch.h"

#include "CommandAllocator.h"

// namespace Helios
// {
//     class CommandAllocatorManager
//     {
//     public:

//         WeakRef<CommandAllocator> GetOrCreateCommandAllocator()
//         {

//         }

//         Scope<CommandAllocator> CreateSingleUseCommandAllocator()
//         {
//             auto cmdalloc = CreateScope<CommandAllocator>();
//             return std::move(cmdalloc);
//         }

//     private:
//         std::unordered_map<std::thread::id, Ref<CommandAllocator>> m_pools;
//         std::mutex m_mutex;
//     };
// }