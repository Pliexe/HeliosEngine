#pragma once

#include "Helios/Resources/Image.h"

#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkSampler.h"
#include "Platform/Vulkan/VkWrappers.h"

namespace Helios::Internal
{

    class VkImage final : public Helios::Image, public BaseVkSyncableResource
    {
      public:
        VkImage(const Ref<VkGraphicsDevice>& device, Usage usage, Format format, std::uint32_t width, std::uint32_t height);
        VkImage(const Ref<VkGraphicsDevice>& device, ::VkImage swapchainImage, VkFormat swapchainFormat, std::uint32_t width, std::uint32_t height);
        ~VkImage();

        void Resize(std::uint32_t width, std::uint32_t height) override;
        void SetData(void* data, uint32 size) override;

        Size GetSize() const override { return {m_width, m_height}; }
        Format GetFormat() const override { return m_format; }
        Usage GetUsage() const override { return m_usage; }
        void* GetID() override;

        VkFormat GetNativeFormat();

        ::VkImageView GetViewImage() { return m_imageView.Get(); }
        ::VkImage GetImage() { return m_image.Get(); }

        bool IsDepth() const override
        {
            return (m_usage & Usage::DepthStencil) == Usage::DepthStencil;
        }
        bool IsSwapchain() const override
        {
            return (m_usage & Usage::Swapchain) == Usage::Swapchain;
        }

        // IVkSyncableResource

        VkPipelineStageFlags GetStage() const override;

      private:
        void Invalidate();
        void Create();
        void CreateSwapchainImageView();
        void Cleanup();

        size_t CalculateSize();

        Vulkan::VkImageWrapper m_image;
        Vulkan::VkImageViewWrapper m_imageView;
        Vulkan::VkDeviceMemoryWrapper m_imageMemory;
        Vulkan::ImGuiTextureIDWrapper m_id;

        VkImageLayout m_defaultLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout m_pendingLayout = VK_IMAGE_LAYOUT_UNDEFINED;

      public:
        VkImageLayout GetDefaultLayout() const { return m_defaultLayout; }
        VkImageLayout GetPendingLayout() const { return m_pendingLayout; }
        void UpdatePendingLayout(VkImageLayout layout)
        {
            m_pendingLayout = layout;
        }

        Ref<VkSampler> m_sampler;

        VkFormat m_swapchainFormat;
    };
} // namespace Helios::Internal