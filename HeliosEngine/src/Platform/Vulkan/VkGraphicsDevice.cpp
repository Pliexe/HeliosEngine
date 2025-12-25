#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "GLFW/glfw3.h"
#include "Helios/Resources/Shader.h"
#include "Helios/Resources/Texture.h"
#include "Platform/Vulkan/Buffers/VkIndexBuffer.h"
#include "Platform/Vulkan/Buffers/VkVertexBuffer.h"
#include "Platform/Vulkan/CommandList/VkCommandList.h"
#include "Platform/Vulkan/GraphicsPipeline/VkShaderCache.h"
#include "Platform/Vulkan/Shader/VkGeometryShader.h"
#include "Platform/Vulkan/Shader/VkPixelShader.h"
#include "Platform/Vulkan/Shader/VkVertexShader.h"
#include "Platform/Vulkan/Sync/VkSyncTracker.h"
#include "Platform/Vulkan/VkContext.h"
#include "Platform/Vulkan/VkImage.h"
#include "Platform/Vulkan/VkRenderPassCache.h"
#include "VkGraphicsDevice.h"
#include "Helios/Core/Application.h"
#include "Helios/Core/Exceptions.h"
#include "Platform/GLFW/GLFWManager.h"
#include "pch.h"
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vulkan/vulkan_core.h>

#ifdef HELIOS_PLATFORM_LINUX
#include <vulkan/vulkan_wayland.h>
#endif

namespace Helios::Internal
{

#pragma region Impl Struct

    struct VkGraphicsDevice::Impl
    {
        std::vector<VkContext*> m_contexts;
        std::vector<Ref<VkCommandList>> m_transientCommandLists;
        std::vector<SyncTicket> m_transientCommandListsTickets;
    };

#pragma endregion

#pragma region Vulkan Instance Helpers

    bool checkValidationLayerSupport(const std::vector<const char *> validationLayers)
    {

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers)
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

    std::vector<const char *> getRequiredExtensions(bool enablevalidationLayers)
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enablevalidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData)
    {

        std::cerr << "Vulkan: Validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
    }

    void setupDebugMessenger(VkInstance vkInstance, VkDebugUtilsMessengerEXT& debugMessenger)
    {
        // if (!s_EnableValidationLayers)
        //     return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        HL_EXCEPTION(CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS, "Failed to set up debug messenger!");
    }

#pragma endregion

#pragma region Pick Physical Device

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indecies;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indecies.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
            {
                indecies.presentFamily = i;
            }

            if (indecies.IsCompleted())
            {
                break;
            }
        }

        return indecies;
    }

    bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indecies = FindQueueFamilies(device, surface);

        return indecies.IsCompleted();
    }

    void pickPhysicalDeviceForSurface(VkInstance vkInstance, VkSurfaceKHR surface, VkPhysicalDevice& physicalDevice)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

        HL_EXCEPTION(deviceCount == 0, "Failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device, surface))
            {
                physicalDevice = device;
                break;
            }
        }

        HL_EXCEPTION(physicalDevice == VK_NULL_HANDLE, "Failed to find a suitable GPU!");
    }

#pragma endregion

#pragma region Create Logical Device

    QueueFamilyIndices FindQueueFamiliesInternal(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indecies;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indecies.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
            {
                indecies.presentFamily = i;
            }

            if (indecies.IsCompleted())
            {
                break;
            }
        }

        return indecies;
    }

    QueueFamilyIndices VkGraphicsDevice::FindQueueFamilies(VkSurfaceKHR surface) { return FindQueueFamiliesInternal(m_physicalDevice, surface); }

    void createLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, bool enabledValidationLayers, ValidationLayers& validationLayers, DeviceExtensions& deviceExtensions, VkDevice& device)
    {
        QueueFamilyIndices indices = FindQueueFamiliesInternal(physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        
        for (uint32_t queueFamiliy : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamiliy;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        
        VkDeviceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

        if (enabledValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else createInfo.enabledLayerCount = 0;

        HL_EXCEPTION(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS, "Failed to create logical device!")
    }

    uint32_t FindGraphicsQueueFamilyOnly(VkPhysicalDevice device)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                return i;
        }

        throw std::runtime_error("No graphics queue family found!");
    }

    void createDeviceQueues(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue& graphicsQueue)
    {
        auto indices = FindGraphicsQueueFamilyOnly(physicalDevice);
        vkGetDeviceQueue(device, indices, 0, &graphicsQueue);

        // TODO: Defer this to VkContext since its Context specific
        // vkGetDeviceQueue(s_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
    }

#pragma endregion

    bool VkGraphicsDevice::Initialize()
    { 
        HL_EXCEPTION(!GLFWManager::Initialize(), "Unable to start GLFW!");

        UpdateState(State::Initializing);
        
        HL_EXCEPTION(s_EnableValidationLayers && !checkValidationLayerSupport(s_ValidationLayers), "Validation layers requested, but not available!");
        
        if (s_vulkanInstance == VK_NULL_HANDLE)
        {
#pragma region Check extensions
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> extensionsTmp(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsTmp.data());

            uint32_t apiVersion;
            vkEnumerateInstanceVersion(&apiVersion);
            std::cout << std::format("Vulkan API Version: {}.{}.{}\n",
                VK_VERSION_MAJOR(apiVersion),
                VK_VERSION_MINOR(apiVersion),
                VK_VERSION_PATCH(apiVersion));
            
            std::cout << "\nVulkan: Available extensions:\n";

            for (const auto &extension : extensionsTmp)
            {
                std::cout << '\t' << extension.extensionName << std::endl;
            }

            std::cout << std::endl << std::endl;

#ifdef HELIOS_PLATFORM_LINUX
            if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND)
            {
                bool waylandSupported = std::any_of(
                    extensionsTmp.begin(), extensionsTmp.end(),
                    [](const VkExtensionProperties &ex) {
                        return std::strcmp(ex.extensionName, "VK_KHR_wayland_surface") == 0;
                    }
                );
    
                HL_EXCEPTION(!waylandSupported, "Fatal! VK_KHR_wayland_surface is missing from available Vulkan extensions. Cannot create a Wayland surface.");
            }
#endif

            
#pragma endregion

            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = Helios::Application::GetInstance().GetApplicationName().data();
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "HeliosEngine";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;

            auto extensions = getRequiredExtensions(s_EnableValidationLayers);

            std::cout << "Requested:" << std::endl;
            for(auto& e : extensions) std::cout << "\t" << e << std::endl;

            // Check if any extension is missing
            for(auto& e : extensions)
            {
                if (std::find_if(extensionsTmp.begin(), extensionsTmp.end(), [e](const VkExtensionProperties& ex) { return std::strcmp(ex.extensionName, e) == 0; }) == extensionsTmp.end())
                {
                    HL_EXCEPTION(true, "Fatal! Extension " + std::string(e) + " is missing from available Vulkan extensions.");
                }
            }

#ifdef HELIOS_PLATFORM_MAC
            extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            if (s_EnableValidationLayers)
            {
                VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

                createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
                createInfo.ppEnabledLayerNames = s_ValidationLayers.data();

                populateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
            }
            else
            {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }

            HL_EXCEPTION(vkCreateInstance(&createInfo, nullptr, &s_vulkanInstance) != VK_SUCCESS, "Failed to create Vulkan Instance!");

            std::cout << "\nVulkan: Instance created successfully!\n"
                        << std::endl;

        }
    
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* tempWindow = glfwCreateWindow(1, 1, "awdawd", nullptr, nullptr);
        HL_EXCEPTION(tempWindow == nullptr, "Failed to create temporary window!");
        VkSurfaceKHR tempSurface = VK_NULL_HANDLE;
        VkResult result = glfwCreateWindowSurface(s_vulkanInstance, tempWindow, nullptr, &tempSurface);
        HL_EXCEPTION(result != VK_SUCCESS, "Failed to create window surface!");
            
        pickPhysicalDeviceForSurface(s_vulkanInstance, tempSurface, m_physicalDevice);
        createLogicalDevice(m_physicalDevice, tempSurface, s_EnableValidationLayers, s_ValidationLayers, s_DeviceExtensions, m_device);
        
        vkDestroySurfaceKHR(s_vulkanInstance, tempSurface, nullptr);
        glfwDestroyWindow(tempWindow);

        
        createDeviceQueues(m_physicalDevice, m_device, m_graphicsQueue);

        m_graphicsQueueFamilyIndex = FindGraphicsQueueFamilyOnly(m_physicalDevice);

        // Get physical device propeties
        vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDevicePropeties);
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_physicalDeviceMemoryProperties);

        m_SingleUseCommandPool = CreateRef<VkCommandAllocator>(shared_from_this());
        
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(m_device, &fenceInfo, nullptr, &m_singleUseSubmitFinished) != VK_SUCCESS) {
            HL_EXCEPTION(true, "failed to create fences!");
        }

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_singleUseSubmitSemaphore) != VK_SUCCESS)
        {
            HL_EXCEPTION(true, "failed to create semaphores!");
        }

        s_devicesInUse++;

        m_impl = CreateRef<Impl>();

        m_renderPassCache = CreateRef<VkRenderPassCache>(shared_from_this());
        m_framebufferCache = CreateRef<VkFramebufferCache>(shared_from_this());
        m_syncTracker = CreateRef<VkSyncTracker>(m_device);
        m_descriptorSetLayoutCache = CreateRef<VkDescriptorSetLayoutCache>(shared_from_this());
        m_graphicsPipelineCache = CreateRef<VkGraphicsPipelineCache>(shared_from_this());
        m_descriptorSetCache = CreateRef<VkDescriptorSetCache>();
        m_shaderCache = CreateRef<VkShaderCache>();
        m_descriptorSetCache->Init(shared_from_this());
        m_syncPool = CreateRef<VkSyncPool>(shared_from_this());

        UpdateState(State::Initialized);

        return true;
    }

    void VkGraphicsDevice::Shutdown()
    {
        UpdateState(State::Terminating);
        {
            std::lock_guard<std::mutex> lock(m_graphicsQueueMutex); 
            vkDeviceWaitIdle(m_device);
        }

        vkWaitForFences(m_device, 1, &m_singleUseSubmitFinished, VK_TRUE, UINT64_MAX);

		// GetAllocatorForThread().Reset(); // Not needed it will destroy it on deconstructor

        m_allocators.clear();

        {
            m_renderPassCache.reset();
            m_framebufferCache.reset();
            m_syncTracker.reset();
            m_descriptorSetLayoutCache.reset();
            m_graphicsPipelineCache.reset();
            m_syncPool.reset();
            m_descriptorSetCache->Shutdown(); // First needs to run shutdown for pools to get removed!!!
            m_descriptorSetCache.reset();
        }
    }



    void VkGraphicsDevice::RegisterTransientCommandList(Ref<NativeCommandList> cmdList)
    {
        std::lock_guard<std::mutex> lock(m_transientCommandListMutex);
        auto vkCmdList = std::static_pointer_cast<VkCommandList>(cmdList);

        m_impl->m_transientCommandLists.push_back(vkCmdList);
        m_impl->m_transientCommandListsTickets.push_back(vkCmdList->GetLastTicket());
    }

    VkCommandBuffer VkGraphicsDevice::LegacyBeginSingleTimeCommands()
    {            
        std::lock_guard<std::mutex> lock(m_transientPoolMutex);
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_SingleUseCommandPool->m_commandPool; // transient pool
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void VkGraphicsDevice::LegacyEndSingleTimeCommands(VkCommandBuffer cmd)
    {
        std::lock_guard<std::mutex> lock(m_transientPoolMutex);
        vkWaitForFences(m_device, 1, &m_singleUseSubmitFinished, VK_TRUE, UINT64_MAX);
        vkResetFences(m_device, 1, &m_singleUseSubmitFinished);
        
        vkEndCommandBuffer(cmd);

        m_graphicsQueueMutex.lock();

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;
        // submitInfo.signalSemaphoreCount = 1;
        // submitInfo.pSignalSemaphores = &m_singleUseSubmitSemaphore;

        vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_singleUseSubmitFinished);
        vkQueueWaitIdle(m_graphicsQueue);

        vkFreeCommandBuffers(m_device, m_SingleUseCommandPool->m_commandPool, 1, &cmd);

        m_graphicsQueueMutex.unlock();
    }

    void VkGraphicsDevice::End()
    {
        for (auto it = m_contexts.begin(); it != m_contexts.end(); ++it)
        {
            (*it)->End(); 
        }


        m_framebufferCache->Step();
        m_renderPassCache->Step();
        m_descriptorSetLayoutCache->Step();
        m_graphicsPipelineCache->Step();
        m_shaderCache->Step();
        m_syncTracker->Step();
        m_cmdQueueSemaphores.clear();
        m_syncPool->Step();
    }
    
    void VkGraphicsDevice::Begin()
    {

        if (!m_impl->m_transientCommandListsTickets.empty())
        {
            m_syncPool->Wait(m_impl->m_transientCommandListsTickets);
            m_impl->m_transientCommandListsTickets.clear();
            m_impl->m_transientCommandLists.clear();
        }

        for (auto it = m_contexts.begin(); it != m_contexts.end(); ++it)
        {
            (*it)->Begin();
        }

        m_descriptorSetCache->Prepare();
    }

    Ref<UnsafeUniformBuffer> VkGraphicsDevice::CreateUniformBuffer(std::uint32_t size) 
    {
        return CreateRef<VkUniformBuffer>(shared_from_this(), size);
    }

    Ref<UnsafeVertexBuffer> VkGraphicsDevice::CreateVertexBuffer(const void *data, uint32_t size, BufferUsage usage)
    {
        std::cout << "Creating vertex buffer: " << std::to_string(size) << std::endl;
        return CreateRef<VkVertexBuffer>(shared_from_this(), data, size, usage);
    }

    Ref<IndexBuffer> VkGraphicsDevice::CreateIndexBuffer(uint32_t* indices, uint32_t count, BufferUsage usage)
    {
        return CreateRef<VkIndexBuffer>(shared_from_this(), indices, count, usage);
    }

    Ref<Texture2D> VkGraphicsDevice::CreateTexture2D(const std::filesystem::path &path)
    {
        return CreateRef<Texture2D>(std::static_pointer_cast<GraphicsDevice>(shared_from_this()), path);
    }

    Ref<Texture2D> VkGraphicsDevice::CreateTexture2D(uint32_t width, uint32_t height)
    {
        return CreateRef<Texture2D>(std::static_pointer_cast<GraphicsDevice>(shared_from_this()), width, height);
    }

    Ref<VertexShader> VkGraphicsDevice::CreateVertexShader(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        VertexShader::Validate(shaderModules);

        return CreateRef<VkVertexShader>(shared_from_this(), shaderModules); 
    }
    Ref<VertexShader> VkGraphicsDevice::CreateVertexShader(const std::filesystem::path &path, InputLayouts inputLayouts)
    {
        return CreateRef<VkVertexShader>(shared_from_this(), path, inputLayouts);
    }
    
    Ref<PixelShader> VkGraphicsDevice::CreatePixelShader(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        PixelShader::Validate(shaderModules);

        return CreateRef<VkPixelShader>(shared_from_this(), shaderModules);
    }
    Ref<PixelShader> VkGraphicsDevice::CreatePixelShader(const std::filesystem::path &path)
    {
        return CreateRef<VkPixelShader>(shared_from_this(), path);
    }
    
    Ref<GeometryShader> VkGraphicsDevice::CreateGeometryShader(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        GeometryShader::Validate(shaderModules);

        return CreateRef<VkGeometryShader>(shared_from_this(), shaderModules);
    }
    Ref<GeometryShader> VkGraphicsDevice::CreateGeometryShader(const std::filesystem::path &path)
    {
        return CreateRef<VkGeometryShader>(shared_from_this(), path);
    }

    Ref<Image> VkGraphicsDevice::CreateImage(Image::Usage usage, Image::Format format, std::uint32_t width, std::uint32_t height)
    {
        HL_EXCEPTION((usage & Image::Usage::Swapchain) == Image::Usage::Swapchain, "You can't create Swapchain type image. A Swapchain image is an internal resource owned by GPU/OS!");
        return CreateRef<VkImage>(shared_from_this(), usage, format, width, height);
    }

    struct ThreadAllocatorGuard
    {
        WeakRef<VkGraphicsDevice> m_device;

        ~ThreadAllocatorGuard()
        {
            if (auto device = m_device.lock())
            {
                std::lock_guard lock(device->m_allocatorMutex);
                device->m_allocators.erase(std::this_thread::get_id());
            }
        }
    };

    CommandAllocator& VkGraphicsDevice::GetAllocatorForThread()
    {
        auto tid = std::this_thread::get_id();
        std::lock_guard lock(m_allocatorMutex);

        auto [it, inserted] = m_allocators.try_emplace(
            tid, shared_from_this()
        );

        thread_local std::unordered_map<VkGraphicsDevice*, ThreadAllocatorGuard> s_guards;
        if (inserted)
            s_guards.emplace(this, shared_from_this());
        
        return it->second;
    }
    VkGraphicsDevice::~VkGraphicsDevice()
    {
        if (is_initialized && !is_shutdown)
            Shutdown();

        vkDestroyFence(m_device, m_singleUseSubmitFinished, nullptr);
        vkDestroySemaphore(m_device, m_singleUseSubmitSemaphore, nullptr);

        vkDestroyDevice(m_device, nullptr);
        s_devicesInUse--;

        UpdateState(State::Terminated);
        is_initialized = false;

        if (s_vulkanInstance != VK_NULL_HANDLE && s_devicesInUse <= 0)
        {
            if (s_EnableValidationLayers)
            {
                DestroyDebugUtilsMessengerEXT(s_vulkanInstance, s_DebugMessenger, nullptr);
            }

            vkDestroyInstance(s_vulkanInstance, nullptr);
            s_vulkanInstance = VK_NULL_HANDLE;
        }
    }

    SyncTicket VkGraphicsDevice::Submit(const SubmitInfo& info, const SubmitResourceInfo& resourceInfo)
    {
        std::lock_guard<std::mutex> lock(m_graphicsQueueMutex);

        if (info.dependency.has_value())
            m_syncPool->Wait(info.dependency.value());

        std::vector<SubmitWait> submitWait;
        submitWait.reserve(resourceInfo.readResources.size() + resourceInfo.writeResources.size());

        for (auto& resource : resourceInfo.readResources)
        {
            if (!resource->GetLock().try_lock())
            {
                HL_EXCEPTION(resource->GetThreadID() == std::this_thread::get_id(), "Cannot submit resource that is being used by the same thread!");
                resource->GetLock().lock();
            }

            if (resource->HasWrite())
                submitWait.push_back(SubmitWait{ resource->GetWrite(), resource->GetStage() });
        }

        for (auto& resource : resourceInfo.writeResources)
        {
            if (!resource->GetLock().try_lock())
            {
                HL_EXCEPTION(resource->GetThreadID() == std::this_thread::get_id(), "Cannot submit resource that is being used by the same thread!");
                resource->GetLock().lock();
            }

            if (resource->HasWrite())
                submitWait.push_back(SubmitWait{ resource->GetWrite(), resource->GetStage() });

            if (resource->HasRead())
            {
                for (auto& ticket : resource->GetRead())
                    submitWait.push_back(SubmitWait{ ticket, resource->GetStage() });
            }
        }

        std::sort(submitWait.begin(), submitWait.end(), [](const SubmitWait& a, const SubmitWait& b) {
            return a.ticket.id < b.ticket.id;
        });

        auto it = std::unique(submitWait.begin(), submitWait.end(), [](SubmitWait& a, const SubmitWait& b) {
            if (a.ticket.id == b.ticket.id) {
                a.stage |= b.stage;
                return true;
            }
            return false;
        });
        submitWait.erase(it, submitWait.end());

        auto resolved = m_syncPool->ResolveSubmitWait(submitWait);

        std::vector<VkSemaphore> semaphores = info.waitSemaphores;
        std::vector<VkPipelineStageFlags> waitStages = info.waitStages;

        semaphores.insert(semaphores.end(), resolved.binarySemaphores.begin(), resolved.binarySemaphores.end());
        waitStages.insert(waitStages.end(), resolved.stages.begin(), resolved.stages.end());

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = semaphores.size();
        submitInfo.pWaitSemaphores = semaphores.data();
        
        std::vector<VkSemaphore> signalSemaphores;
        if (info.signalSemaphore != VK_NULL_HANDLE) signalSemaphores.push_back(info.signalSemaphore);
        signalSemaphores.push_back(resolved.submitSemaphore);


        submitInfo.signalSemaphoreCount = signalSemaphores.size();
        submitInfo.pSignalSemaphores = signalSemaphores.data();
        
        submitInfo.pCommandBuffers = info.commandBuffers;
        submitInfo.commandBufferCount = info.commandBufferCount;
        submitInfo.pWaitDstStageMask = waitStages.data();

        VkFence fenceToSignal = resolved.submitFence;
        // vkResetFences(m_device, 1, &fenceToSignal); // Handled by SyncPool::Acquire
        
        HL_EXCEPTION(vkQueueSubmit(m_graphicsQueue, 1, &submitInfo,
                                   resolved.submitFence) != VK_SUCCESS,
                     "Failed to submit command buffer!");

        for (auto& resource : resourceInfo.readResources)
        {
            resource->Clean();
            resource->AddRead(resolved.ticket);
            resource->GetLock().unlock();
        }

        for (auto& resource : resourceInfo.writeResources)
        {
            resource->Clean();
            resource->SetWrite(resolved.ticket);
            resource->GetLock().unlock();
        }

        m_descriptorSetCache->UpdateFrameTicket(resolved.ticket);
            
        return resolved.ticket;
    }
} // namespace Helios::Internal