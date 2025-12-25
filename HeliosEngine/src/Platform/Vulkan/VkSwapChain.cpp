#include "VkSwapChain.h"

#include "VkContext.h"

namespace Helios::Internal
{
void VkSwapChain::Resize(uint32_t width, uint32_t height)
{
    {
        std::lock_guard<std::mutex> lock(m_device->m_graphicsQueueMutex);
        vkDeviceWaitIdle(*m_device);
    }
    CleanupSwapchain();
    m_width = width;
    m_height = height;
    CreateSwapchain();
    // CreateImageViews();
}

VkSurfaceFormatKHR
ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        // if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        // availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }

    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR)
            return availablePresentMode;
    }

    // Fallback to the first available if none of the preferred ones exist
    std::cerr << "Warning: Preferred present modes not available. Falling back "
                 "to first supported mode.\n";
    return availablePresentModes[0];
}

VkSwapChain::VkSwapChain(const Ref<VkGraphicsDevice>& device,
                         VkContext* context)
{
    m_context = context;
    m_device = device;
    m_width = 1290;
    m_height = 900;

    CreateSwapchain();
}

VkSwapChain::~VkSwapChain() { Cleanup(); }

VkSwapChain::SwapChainSupportDetails
VkSwapChain::QuerySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device, m_context->GetSurfaceKHR(), &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_context->GetSurfaceKHR(),
                                         &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_context->GetSurfaceKHR(),
                                             &formatCount,
                                             details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, m_context->GetSurfaceKHR(), &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, m_context->GetSurfaceKHR(), &presentModeCount,
            details.presentModes.data());
    }

    return details;
}
VkExtent2D
VkSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        // Get current window size
        VkExtent2D actualExtent = {static_cast<uint32_t>(m_width),
                                   static_cast<uint32_t>(m_height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                       capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
void VkSwapChain::CreateSwapchain()
{
    std::cout << "\n\nCreating Swapchain!\n";

    SwapChainSupportDetails swapChainSupport =
        QuerySwapChainSupport(m_device->GetNativePhysicalDevice());
    std::cout << "Supported present modes:\n";
    for (auto mode : swapChainSupport.presentModes)
    {
        switch (mode)
        {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            std::cout << "  IMMEDIATE\n";
            break;
        case VK_PRESENT_MODE_MAILBOX_KHR:
            std::cout << "  MAILBOX\n";
            break;
        case VK_PRESENT_MODE_FIFO_KHR:
            std::cout << "  FIFO\n";
            break;
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            std::cout << "  FIFO_RELAXED\n";
            break;
        default:
            std::cout << "  Unknown mode: " << mode << "\n";
            break;
        }
    }

    VkSurfaceFormatKHR surfaceFormat =
        ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
        ChooseSwapPresentMode(swapChainSupport.presentModes);

    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    m_MinImageCount =
        std::max(2u, swapChainSupport.capabilities.minImageCount + 1);
    ;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        m_MinImageCount > swapChainSupport.capabilities.maxImageCount)
    {
        m_MinImageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_context->GetSurfaceKHR();

    createInfo.minImageCount = m_MinImageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices =
        m_device->FindQueueFamilies(m_context->GetSurfaceKHR());
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result;

    {
        std::lock_guard<std::mutex> lock(m_device->m_graphicsQueueMutex);
        vkDeviceWaitIdle(*m_device);
    }
    if ((result = vkCreateSwapchainKHR(*m_device, &createInfo, nullptr,
                                       &m_swapchain)) != VK_SUCCESS)
    {
        HL_EXCEPTION(true,
                     "Failed to create swap chain!" + std::to_string(result));
    }

    std::vector<::VkImage> swapchainImages;

    vkGetSwapchainImagesKHR(*m_device, m_swapchain, &m_MinImageCount, nullptr);
    swapchainImages.resize(m_MinImageCount);
    vkGetSwapchainImagesKHR(*m_device, m_swapchain, &m_MinImageCount,
                            swapchainImages.data());

    m_SwapchainImageFormat = surfaceFormat.format;
    m_SwapchainExtent = extent;
    m_width = extent.width;
    m_height = extent.height;

    std::cout << "Swapchain images: " << swapchainImages.size() << "\n";

    int i = 0;
    for (auto image : swapchainImages)
    {
        std::cout << "Image " << i << ": " << image << "\n";
        auto img = CreateRef<VkImage>(m_device, image, m_SwapchainImageFormat,
                                      m_width, m_height);
        m_swapchainImages.push_back(img);
        i++;
    }
}

void VkSwapChain::CleanupSwapchain()
{
    for (auto& img : m_swapchainImages)
        img.reset();
    m_swapchainImages.clear();

    vkDestroySwapchainKHR(*m_device, m_swapchain, nullptr);
    m_swapchain = VK_NULL_HANDLE;
}

void VkSwapChain::Cleanup()
{
    for (auto& img : m_swapchainImages)
        img.reset();
    m_swapchainImages.clear();
}
} // namespace Helios::Internal