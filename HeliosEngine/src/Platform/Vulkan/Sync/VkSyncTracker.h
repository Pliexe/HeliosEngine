#pragma once

#include "pch.h"
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{
    class VkSyncTracker
    {
    public:
        // No need to keep shared ptr since it lives inside graphics device so this class will only get destroyed before graphics device is!
        VkSyncTracker(VkDevice device) : m_device(device) { }
    
        void RegisterCallback(VkFence fence, std::function<void(VkFence)> func)
        {
            std::lock_guard lock(m_mutex);
            m_callbacks[fence].push_back(std::move(func));
        }

        void RemoveRegisterCallbacks(const std::vector<VkFence>& fence)
        {
        }

        void Step()
        {
            using namespace std::chrono;

            auto now = steady_clock::now();
            if (now < m_nextCheck)
                return;

            const uint32_t maxSkip = 30;
            const uint32_t minSkip = 1;

            uint32_t fenceCount = 0;

            {
                std::lock_guard lock(m_mutex);
                fenceCount = (uint32_t)m_callbacks.size();

                if (fenceCount > 100 && m_skipFrames < maxSkip)
                    m_skipFrames++;
                else if (fenceCount < 30 && m_skipFrames > minSkip)
                    m_skipFrames--;

                m_nextCheck = now + milliseconds(16 * m_skipFrames);
            }

            if (fenceCount == 0)
                return;

            std::vector<VkFence> fencesToCheck;
            {
                std::lock_guard lock(m_mutex);
                fencesToCheck.reserve(m_callbacks.size());
                for (auto& [fence, _] : m_callbacks)
                    fencesToCheck.push_back(fence);
            }

            for (VkFence fence : fencesToCheck)
            {
                VkResult res = vkGetFenceStatus(m_device, fence);
                if (res == VK_SUCCESS)
                {
                    std::vector<std::function<void(VkFence)>> callbacksToRun;
                    {
                        std::lock_guard lock(m_mutex);
                        auto it = m_callbacks.find(fence);
                        if (it != m_callbacks.end())
                        {
                            callbacksToRun = std::move(it->second);
                            m_callbacks.erase(it);
                        }
                    }

                    for (auto& cb : callbacksToRun)
                        cb(fence);
                }
            }
        }

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        std::unordered_map<VkFence, std::vector<std::function<void(VkFence)>>> m_callbacks;
        std::mutex m_mutex;

        std::chrono::steady_clock::time_point m_nextCheck = std::chrono::steady_clock::now();
        uint32_t m_skipFrames = 1;
    };
}
