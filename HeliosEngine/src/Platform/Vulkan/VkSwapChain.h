#pragma once

#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkImage.h"
#include "pch.h"

#include "VkIncludes.h"

namespace Helios::Internal
{
    class VkContext;
    class VkSwapChain final 
    {
    public:

        VkSwapChain(const VkSwapChain&) = delete;
        VkSwapChain& operator =(VkSwapChain&) = delete;
    
        const Ref<Image> GetImage() const { return m_swapchainImages[m_currentImageIndex]; }
        uint32_t GetSwapchainImageCount() const { return m_swapchainImages.size(); }

        uint32_t GetMinImageCount() const { return m_MinImageCount; }

        void Resize(uint32_t width, uint32_t height);

        VkSwapChain(const Ref<VkGraphicsDevice>& device, VkContext* context);
        ~VkSwapChain();
        
    private:

        friend class VkContext;
        friend class VkMainRenderPass;

        static Ref<VkSwapChain> Create(const Ref<VkGraphicsDevice>& device, VkContext* context)
        {
            return CreateRef<VkSwapChain>(device, context);
        }

        struct SwapChainSupportDetails
        {
             VkSurfaceCapabilitiesKHR capabilities;
             std::vector<VkSurfaceFormatKHR> formats;
			 std::vector<VkPresentModeKHR> presentModes;
        };

        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        void CreateSwapchain();
        void CreateImageViews();

        void CleanupSwapchain();
        void Cleanup();

        VkSwapchainKHR m_swapchain;
        std::vector<Ref<VkImage>> m_swapchainImages;
        // std::vector<VkImage> m_SwapchainImages;
        // std::vector<VkImageView> m_SwapchainImageView;
        VkFormat m_SwapchainImageFormat;
		VkExtent2D m_SwapchainExtent;
        VkContext* m_context;
        std::uint32_t m_currentImageIndex = 0;

        std::function<void()> m_cbOnResize;

        uint32_t m_width, m_height;

        uint32_t m_MinImageCount = 2;

        Ref<VkGraphicsDevice> m_device;
    };
}