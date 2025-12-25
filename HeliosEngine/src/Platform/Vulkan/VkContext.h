#pragma once

#include "Helios/Graphics/GPURenderPass.h"
#include "Helios/Math/Vector.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"

#include "Helios/Core/Application.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/GraphicsContext.h"
#include "Platform/Vulkan/GraphicsPipeline/VKDescriptorCache.h"
#include "pch.h"


#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "VkCommandAllocator.h"
#include "VkSwapChain.h"

// I HATE FORMAT ON SAVE, IT RUINED MY CODE IN THIS FILE

namespace Helios::Internal
{
class VkContext final : public GraphicsContext
{

  public:
    ::VkRenderPass GetNativeRenderPass() const
    {
        auto& renderPass = m_graphicsDevice->GetRenderPassCache().Acquire(
            GetRenderPassBeginInfo());
        assert(renderPass.Get() != VK_NULL_HANDLE);
        return renderPass.Get();
    }

    void WaitAllSync() override
    {
        m_graphicsDevice->GetSyncPool().Step();
        std::lock_guard<std::mutex> lock(m_graphicsDevice->m_graphicsQueueMutex);
        vkDeviceWaitIdle(m_graphicsDevice->GetNativeDevice());
    }

    Ref<Image> GetSwapchainImage() const override
    {
        return m_swapChain->GetImage();
    }

    void ResizeSwapchain(const Size& size) override
    {
        m_newSwapchainSize = size;
    }

    const RenderPassBeginInfo GetRenderPassBeginInfo() const override
    {
        RenderPassBeginInfo passInfo{};

        passInfo.attachmentSpecifications.push_back({
            .buffer = m_swapChain->GetImage(),
            .clearColor = Color::Black,
        });

        return passInfo;
    }

    VkContext(Ref<VkGraphicsDevice> graphicsDevice,
              std::function<Size()> getWindowSize)
    {
        m_graphicsDevice = graphicsDevice;
        m_getWindowSize = getWindowSize;
    }

    ~VkContext()
    {
        for (std::size_t i = 0, n = m_graphicsDevice->m_contexts.size(); i < n;
             ++i)
        {
            if (m_graphicsDevice->m_contexts[i] == this)
            {
                m_graphicsDevice->m_contexts.erase(
                    m_graphicsDevice->m_contexts.begin() + i);
                break;
            }
        }

        {
            std::lock_guard<std::mutex> lock(m_graphicsDevice->m_graphicsQueueMutex);
            vkDeviceWaitIdle(m_graphicsDevice->GetNativeDevice());
        }
        vkWaitForFences(m_graphicsDevice->GetNativeDevice(),
                        m_frameData.fenceInFlight.size(),
                        m_frameData.fenceInFlight.data(), VK_TRUE, UINT64_MAX);

        for (size_t i = 0, n = Graphics::GetMaxFramesInFlight(); i < n; i++)
        {
            // m_frameData.commandList[i].reset();
            vkDestroySemaphore(m_graphicsDevice->GetNativeDevice(),
                               m_frameData.imageAvailableSemaphores[i],
                               nullptr);
            vkDestroyFence(m_graphicsDevice->GetNativeDevice(),
                           m_frameData.fenceInFlight[i], nullptr);
        }

        for (size_t i = 0, n = m_swapChain->GetSwapchainImageCount(); i < n;
             i++)
        {
            vkDestroySemaphore(m_graphicsDevice->GetNativeDevice(),
                               m_frameData.renderingFinishedSemaphore[i],
                               nullptr);
        }

        m_swapChain.reset();
        vkDestroySurfaceKHR(m_graphicsDevice->GetVkInstance(), m_Surface,
                            nullptr);
    }

    static uint32_t GetGraphicsQueueFamilyIndex()
    {
        return s_GraphicsQueueFamilyIndex;
    }

    VkSwapChain& GetSwapChain() const { return *m_swapChain; }

    VkSurfaceKHR GetSurfaceKHR() const { return m_Surface; }

    void Begin() override
    {
        auto device = m_graphicsDevice->GetNativeDevice();

        VkResult result = VK_TIMEOUT;
        VkSemaphore currentSemaphore =
            m_frameData
                .imageAvailableSemaphores[m_frameData.currentAcquireSyncIndex];
        
        // We don't use acquireFences anymore in the new system
        VkFence currentFence = VK_NULL_HANDLE;

        while (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            result = vkAcquireNextImageKHR(
                device, m_swapChain->m_swapchain, UINT64_MAX, currentSemaphore,
                currentFence, &m_swapChain->m_currentImageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                m_swapChain->Resize(m_newSwapchainSize->width,
                                    m_newSwapchainSize->height);
                m_newSwapchainSize.reset();
            }
            else if (result == VK_SUBOPTIMAL_KHR)
            {
                std::cout << "RES: SUBOPTIMAL (continuing)" << std::endl;
                break;
            }
            else
            {
                HL_EXCEPTION(result != VK_SUCCESS,
                             "Failed to acquire next swapchain image!");
            }
        }

        auto swapImage =
            std::static_pointer_cast<VkImage>(m_swapChain->GetImage());

        // Sync-Submit to bridge acquisition into the ticket system
        VkGraphicsDevice::SubmitInfo submitInfo{};
        submitInfo.waitSemaphores = { currentSemaphore };
        submitInfo.waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.commandBuffers = nullptr;
        submitInfo.commandBufferCount = 0;

        VkGraphicsDevice::SubmitResourceInfo resInfo{};
        resInfo.writeResources = { swapImage };

        m_graphicsDevice->Submit(submitInfo, resInfo);
    }

    void End() override
    {
        auto presentSemaphore = m_frameData.renderingFinishedSemaphore[m_swapChain->m_currentImageIndex];
        auto swapImage = std::static_pointer_cast<VkImage>(m_swapChain->GetImage());

        // if (
        //     swapImage->GetSync().m_writeSemaphore
        //     ==
        //     m_frameData.imageAvailableSemaphores[m_frameData.currentAcquireSyncIndex]
        // )
        // {
        //     // m_frameData.dummyCommandList.reset();

        //     if (m_frameData.dummyCommandList == nullptr)
        //     {
        //         m_frameData.dummyCommandList =
        //             std::static_pointer_cast<VkCommandList>(
        //                 m_graphicsDevice->GetAllocatorForThread()
        //                     .CreateCommandList());
        //     }

        //     RenderPassBeginInfo pass;
        //     pass.attachmentSpecifications.push_back(
        //         RenderPassBeginInfo::AttachmentInfo{
        //             .buffer = swapImage,
        //             .loadOp = RenderPassBeginInfo::LoadOp::Clear});
        //     // pass.attachmentSpecifications.push_back(RenderPassBeginInfo::AttachmentInfo { .buffer = swapImage, .loadOp = RenderPassBeginInfo::LoadOp::Load });

        //     m_frameData.dummyCommandList->Reset();
        //     m_frameData.dummyCommandList->BeginRenderPass(pass);
        //     m_frameData.dummyCommandList->EndRenderPass();
        //     m_frameData.dummyCommandList->Execute();

        // }

        // VkSemaphore writeSemaphore = swapImage->GetSync().m_writeSemaphore;

        // if (writeSemaphore != VK_NULL_HANDLE)
        // {
        //     VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        //     VkSubmitInfo syncSubmit{};

        //     syncSubmit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //     syncSubmit.waitSemaphoreCount = 1;
        //     syncSubmit.pWaitSemaphores = &writeSemaphore;
        //     syncSubmit.pWaitDstStageMask = &waitStage;
        //     syncSubmit.signalSemaphoreCount = 1;
        //     syncSubmit.pSignalSemaphores = &presentSemaphore;
        //     syncSubmit.commandBufferCount = 0;

        //     m_graphicsDevice->m_graphicsQueueMutex.lock();
        //     HL_EXCEPTION(vkQueueSubmit(m_graphicsDevice->GetGraphicsQueue(), 1, &syncSubmit, VK_NULL_HANDLE) != VK_SUCCESS, "Failed to submit sync command buffer!");
        //     m_graphicsDevice->m_graphicsQueueMutex.unlock();
        // }

        VkGraphicsDevice::SubmitInfo submitInfo{};
        submitInfo.signalSemaphore = presentSemaphore;
        submitInfo.commandBuffers = nullptr;
        submitInfo.commandBufferCount = 0;
        
        VkGraphicsDevice::SubmitResourceInfo resInfo{};
        resInfo.readResources = { swapImage };
        
        m_graphicsDevice->Submit(submitInfo, resInfo);
        
        m_graphicsDevice->m_graphicsQueueMutex.lock();

        VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &presentSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapChain->m_swapchain;
        presentInfo.pImageIndices = &m_swapChain->m_currentImageIndex;

        std::cout << "-----------------------\n-------------- START VKSUBMIT "
                     "--------------\n-------------------------------"
                  << std::endl;

        // HL_EXCEPTION(vkQueuePresentKHR(m_PresentQueue, &presentInfo) !=
        // VK_SUCCESS, "Failed to present image! Fatal Error!");
        auto res = vkQueuePresentKHR(m_PresentQueue, &presentInfo);
        std::cout << "-----------------------\n--------------- END VKSUBMIT "
                     "---------------\n-------------------------------"
                  << std::endl;

        auto VkResultToString = [](VkResult r)
        {
            switch (r)
            {
            case VK_SUCCESS:
                return "VK_SUCCESS";
            case VK_SUBOPTIMAL_KHR:
                return "VK_SUBOPTIMAL_KHR";
            case VK_ERROR_OUT_OF_DATE_KHR:
                return "VK_ERROR_OUT_OF_DATE_KHR";
            case VK_ERROR_DEVICE_LOST:
                return "VK_ERROR_DEVICE_LOST";
            // add others you care about
            default:
                return "UNKNOWN_VKRESULT";
            }
        };

        if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
        {
            std::cerr << "Swapchain out of date or suboptimal, recreating...\n";

            Size x = m_getWindowSize();
            vkDeviceWaitIdle(*m_graphicsDevice);
            m_graphicsDevice->m_graphicsQueueMutex.unlock();
            m_swapChain->Resize(x.width, x.height);
            m_graphicsDevice->m_graphicsQueueMutex.lock();
            m_graphicsDevice->GetFramebufferCache().Clear();
            m_graphicsDevice->GetRenderPassCache().Clear();
        }
        else if (res != VK_SUCCESS)
        {

            std::cerr << "vkQueuePresentKHR returned " << res << " ("
                      << VkResultToString(res) << ")\n";
            std::cerr << "presentInfo.waitSemaphoreCount="
                      << presentInfo.waitSemaphoreCount << "\n";
            for (uint32_t i = 0; i < presentInfo.waitSemaphoreCount; ++i)
                std::cerr << " waitSemaphore[" << i
                          << "]=" << presentInfo.pWaitSemaphores[i] << "\n";
            std::cerr << "presentInfo.swapchainCount="
                      << presentInfo.swapchainCount << "\n";
            for (uint32_t i = 0; i < presentInfo.swapchainCount; ++i)
            {
                std::cerr << " swapchain[" << i
                          << "]=" << presentInfo.pSwapchains[i]
                          << ", imageIndex=" << presentInfo.pImageIndices[i]
                          << "\n";
            }
            HL_EXCEPTION(true, "Failed to present image! Fatal Error!");
        }

        static std::uint64_t test = 1;

        // if (test > 20)
        // {
        //     HL_DEBUGBREAK();
        //     test = 0;
        // }
        // test++;

        if (m_newSwapchainSize.has_value())
        {
            m_graphicsDevice->m_graphicsQueueMutex.unlock();
            m_swapChain->Resize(m_newSwapchainSize->width, m_newSwapchainSize->height);
            m_graphicsDevice->m_graphicsQueueMutex.lock();
            m_graphicsDevice->GetFramebufferCache().Clear();
            m_graphicsDevice->GetRenderPassCache().Clear();
            m_newSwapchainSize.reset();
        }

        m_frameData.currentAcquireSyncIndex =
            ++m_frameData.currentAcquireSyncIndex %
            (m_swapChain->GetSwapchainImageCount() + 1);

        m_graphicsDevice->m_graphicsQueueMutex.unlock();
    }

    void SetViewport(uint32_t width, uint32_t height) override
    {
        m_Width = width;
        m_Height = height;
    }

    Ref<Framebuffer> GetFramebuffer() override { return nullptr; }

    uint32_t GetVSyncInterval() override { return m_SwapInterval; }

    void SetVSync(uint32_t interval) override
    {
        m_SwapInterval = interval;
        glfwSwapInterval(interval);
    }

    void BindDefaultFramebuffer() override {}

    bool Init() override
    {
        HL_EXCEPTION(s_EnableValidationLayers && !checkValidationLayerSupport(),
                     "Validation layers requested, but not available!");

        createSurface();

        auto indices = m_graphicsDevice->FindQueueFamilies(m_Surface);
        vkGetDeviceQueue(*m_graphicsDevice, indices.presentFamily.value(), 0,
                         &m_PresentQueue);
        m_swapChain = VkSwapChain::Create(m_graphicsDevice, this);
        setupFrames();

        m_graphicsDevice->m_contexts.emplace_back(this);
        s_ContextCount++;
        return true;
    }

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsCompleted()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData)
    {

        std::cerr << "Vulkan: Validation layer: " << pCallbackData->pMessage
                  << std::endl;

        return VK_FALSE;
    }

  private:
    void setupFrames()
    {
        // m_frameData.commandList.resize(Graphics::GetMaxFramesInFlight());

        // std::vector<VkCommandBuffer> commandBuffers;
        // commandBuffers.resize(Graphics::GetMaxFramesInFlight());

        // auto& cmdPool =
        // static_cast<VkCommandAllocator&>(CommandAllocator::GetForCurrentThread()).m_commandPool;

        // VkCommandBufferAllocateInfo allocInfo{};
        // allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        // allocInfo.commandPool = cmdPool;
        // allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        // allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        // HL_EXCEPTION(vkAllocateCommandBuffers(m_graphicsDevice->GetNativeDevice(),
        // &allocInfo, commandBuffers.data()) != VK_SUCCESS, "Failed to allocate
        // command buffers!");

        // for (auto& frame : m_frameData.commandList)
        // {
        //     frame =
        //     CreateRef<VkCommandList>(std::static_pointer_cast<VkGraphicsDevice>(Graphics::GetMainDevice()),
        //     cmdPool);
        // }

        createSyncObjects();
    }

    uint32_t FindGraphicsQueueFamilyOnly(VkPhysicalDevice device)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                                 nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                                 queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                return i;
        }

        throw std::runtime_error("No graphics queue family found!");
    }

    std::vector<const char*> getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(
            glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (s_EnableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : s_ValidationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    void createSurface()
    {
        HL_EXCEPTION(
            glfwCreateWindowSurface(m_graphicsDevice->GetVkInstance(),
                                    (GLFWwindow*)Application::GetInstance()
                                        .GetWindow()
                                        ->GetNativeWindow(),
                                    nullptr, &m_Surface) != VK_SUCCESS,
            "Failed to create window surface!");
    }

    void createSyncObjects()
    {
        m_frameData.fenceInFlight.resize(Graphics::GetMaxFramesInFlight());

        m_frameData.renderingFinishedSemaphore.resize(
            m_swapChain->GetSwapchainImageCount());

        m_frameData.imageAvailableSemaphores.resize(
            m_swapChain->GetSwapchainImageCount() + 1);
        m_frameData.acquireFences.resize(m_swapChain->GetSwapchainImageCount() +
                                         1);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0, n = Graphics::GetMaxFramesInFlight(); i < n; i++)
        {
            if (vkCreateFence(m_graphicsDevice->GetNativeDevice(), &fenceInfo,
                              nullptr,
                              &m_frameData.fenceInFlight[i]) != VK_SUCCESS)
            {

                HL_EXCEPTION(true, "failed to create semaphores!");
            }
        }

        for (size_t i = 0, n = m_frameData.imageAvailableSemaphores.size();
             i < n; i++)
        {
            if (vkCreateSemaphore(m_graphicsDevice->GetNativeDevice(),
                                  &semaphoreInfo, nullptr,
                                  &m_frameData.imageAvailableSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateFence(*m_graphicsDevice, &fenceInfo, nullptr,
                              &m_frameData.acquireFences[i]) != VK_SUCCESS)
            {

                HL_EXCEPTION(true, "failed to create semaphores!");
            }
        }

        for (size_t i = 0, n = m_frameData.renderingFinishedSemaphore.size();
             i < n; i++)
        {
            if (vkCreateSemaphore(m_graphicsDevice->GetNativeDevice(),
                                  &semaphoreInfo, nullptr,
                                  &m_frameData.renderingFinishedSemaphore[i]) !=
                VK_SUCCESS)
            {

                HL_EXCEPTION(true, "failed to create semaphores!");
            }
        }
    }

  private:
    Ref<Internal::VkGraphicsDevice> m_graphicsDevice;

    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;

    uint32_t m_SwapInterval = 0u;
    uint32_t m_Width, m_Height;

    Ref<VkSwapChain> m_swapChain;

    std::function<Size()> m_getWindowSize;

    struct FrameData
    {
        std::vector<VkFence> fenceInFlight;
        std::vector<VkFence> acquireFences;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderingFinishedSemaphore;
        std::uint32_t currentAcquireSyncIndex = 0u;
    };

    FrameData m_frameData;

    std::optional<Size> m_newSwapchainSize;

    inline static std::mutex m_queueSubmitMutex;

    inline static uint32_t s_GraphicsQueueFamilyIndex = 0;

    inline static uint32_t s_ContextCount = 0;
#ifdef HELIOS_DEBUG
    // inline static bool s_EnableValidationLayers = true; broken rn
    inline static bool s_EnableValidationLayers = true;
#else
    inline static bool s_EnableValidationLayers = false;
#endif

    inline static const std::vector<const char*> s_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"};

    inline static const std::vector<const char*> s_DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
} // namespace Helios::Internal