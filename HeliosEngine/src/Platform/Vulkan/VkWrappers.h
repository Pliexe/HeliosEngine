#pragma once

#include "pch.h"
#include "Helios/Utils/Expected.h"
#include "imgui_impl_vulkan.h"
#include <vulkan/vulkan_core.h>

namespace Helios::Internal::Vulkan
{
    template<typename HandleType>
    struct VulkanCreateTraits;

#define DEFINE_VULKAN_CREATE_TRAIT(HandleType, CreateFunc)                          \
template<>                                                                          \
struct VulkanCreateTraits<HandleType>                                               \
{                                                                                   \
    using Handle = HandleType;                                                      \
                                                                                    \
    template<typename... Args>                                                      \
    static VkResult CreateImpl(VkDevice device, Handle* outHandle, Args&&... args)  \
    {                                                                               \
        return CreateFunc(device, std::forward<Args>(args)..., outHandle);          \
    }                                                                               \
}

    template<typename HandleType, typename Deleter>
    class RAIIWrapper
    {
    public:
        using Traits = VulkanCreateTraits<HandleType>;
    
        RAIIWrapper()
            : m_device(VK_NULL_HANDLE), m_handle(VK_NULL_HANDLE), m_deleter() {}

        RAIIWrapper(VkDevice device, HandleType handle, Deleter deleter = Deleter())
            : m_device(device), m_handle(handle), m_deleter(deleter) {}

        ~RAIIWrapper()
        {
            Reset();
        }

        RAIIWrapper(const RAIIWrapper&) = delete;
        RAIIWrapper& operator=(const RAIIWrapper&) = delete;

        RAIIWrapper(RAIIWrapper&& other) noexcept
            : m_device(other.m_device), m_handle(other.m_handle), m_deleter(std::move(other.m_deleter))
        {
            other.m_handle = VK_NULL_HANDLE;
            other.m_device = VK_NULL_HANDLE;
        }

        template<typename... Args>
        static Utils::Expected<RAIIWrapper, const char*> Create(VkDevice device, Args&&... args)
        {
            HandleType handle = VK_NULL_HANDLE;
            VkResult result = Traits::CreateImpl(device, &handle, std::forward<Args>(args)...);
            if (result != VK_SUCCESS) return "Failed to create Vulkan resource";
            return RAIIWrapper(device, handle);
        }

        RAIIWrapper& operator=(RAIIWrapper&& other) noexcept
        {
            if (this != &other)
            {
                Reset();
                m_device = other.m_device;
                m_handle = other.m_handle;
                m_deleter = std::move(other.m_deleter);

                other.m_handle = VK_NULL_HANDLE;
                other.m_device = VK_NULL_HANDLE;
            }
            return *this;
        }

        void Reset()
        {
            if (m_handle != VK_NULL_HANDLE)
            {
                m_deleter(m_device, m_handle);
                m_handle = VK_NULL_HANDLE;
            }
        }

        bool operator==(HandleType other) const
        {
            return m_handle == other;
        }

        bool operator!=(HandleType other) const
        {
            return m_handle != other;
        }

        bool operator==(std::nullptr_t) const
        {
            if constexpr (std::is_pointer_v<HandleType>)
                return m_handle == nullptr;
            else
                return m_handle == VK_NULL_HANDLE;
        }

        bool operator!=(std::nullptr_t) const
        {
            return !(*this == nullptr);
        }

        HandleType Get() const { return m_handle; }

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        HandleType m_handle = VK_NULL_HANDLE;
        Deleter m_deleter;
    };

    #define DEFINE_VK_DELETER(Name, DestroyFunc)                        \
    struct Name##Deleter                                                \
    {                                                                   \
        void operator()(VkDevice device, Name handle) const             \
        {                                                               \
            if (handle != VK_NULL_HANDLE && device != VK_NULL_HANDLE)   \
            {                                                           \
                DestroyFunc(device, handle, nullptr);                   \
            }                                                           \
        }                                                               \
    }

    DEFINE_VK_DELETER(VkImage, vkDestroyImage);
    DEFINE_VK_DELETER(VkImageView, vkDestroyImageView);
    DEFINE_VK_DELETER(VkDeviceMemory, vkFreeMemory);
    DEFINE_VK_DELETER(VkSampler, vkDestroySampler);
    DEFINE_VK_DELETER(VkShaderModule, vkDestroyShaderModule);
    DEFINE_VK_DELETER(VkRenderPass, vkDestroyRenderPass);
    DEFINE_VK_DELETER(VkFramebuffer, vkDestroyFramebuffer);
    DEFINE_VK_DELETER(VkDescriptorSetLayout, vkDestroyDescriptorSetLayout);
    DEFINE_VK_DELETER(VkBuffer, vkDestroyBuffer);

    DEFINE_VULKAN_CREATE_TRAIT(VkImage, vkCreateImage);
    DEFINE_VULKAN_CREATE_TRAIT(VkImageView, vkCreateImageView);
    DEFINE_VULKAN_CREATE_TRAIT(VkDeviceMemory, vkAllocateMemory);
    DEFINE_VULKAN_CREATE_TRAIT(VkSampler, vkCreateSampler);
    DEFINE_VULKAN_CREATE_TRAIT(VkShaderModule, vkCreateShaderModule);
    DEFINE_VULKAN_CREATE_TRAIT(VkRenderPass, vkCreateRenderPass);
    DEFINE_VULKAN_CREATE_TRAIT(VkFramebuffer, vkCreateFramebuffer);
    DEFINE_VULKAN_CREATE_TRAIT(VkDescriptorSetLayout, vkCreateDescriptorSetLayout);
    DEFINE_VULKAN_CREATE_TRAIT(VkBuffer, vkCreateBuffer);

    struct ImGuiTextureDeleter
    {
        void operator()(VkDevice /*device*/, VkDescriptorSet descriptorSet) const
        {
            if (descriptorSet != VK_NULL_HANDLE)
                ImGui_ImplVulkan_RemoveTexture(descriptorSet);
        }
    };

    using ImGuiTextureIDWrapper = RAIIWrapper<VkDescriptorSet, ImGuiTextureDeleter>;

    using VkImageWrapper = RAIIWrapper<VkImage, VkImageDeleter>;
    using VkImageViewWrapper = RAIIWrapper<VkImageView, VkImageViewDeleter>;
    using VkSamplerWrapper = RAIIWrapper<VkSampler, VkSamplerDeleter>;
    using VkDeviceMemoryWrapper = RAIIWrapper<VkDeviceMemory, VkDeviceMemoryDeleter>;
    using VkBufferWrapper = RAIIWrapper<VkBuffer, VkBufferDeleter>;
    using VkShaderModuleWrapper = RAIIWrapper<VkShaderModule, VkShaderModuleDeleter>;
    using VkRenderPassWrapper = RAIIWrapper<VkRenderPass, VkRenderPassDeleter>;
    using VkFramebufferWrapper =  RAIIWrapper<VkFramebuffer, VkFramebufferDeleter>;
    using VkDescriptorSetLayoutWrapper =  RAIIWrapper<VkDescriptorSetLayout, VkDescriptorSetLayoutDeleter>;
}