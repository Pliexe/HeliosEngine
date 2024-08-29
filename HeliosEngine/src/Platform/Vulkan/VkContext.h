#pragma once

#include "pch.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Core/Application.h"
#include "Helios/Graphics/GraphicsContext.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Helios
{
    class VkContext : public GraphicsContext
    {

    public:
        VkContext()
        {
        }

        ~VkContext()
        {
            vkDestroySwapchainKHR(s_Device, s_swapchain, nullptr);
            vkDestroySurfaceKHR(s_Instance, s_Surface, nullptr);
            if (s_Instance != VK_NULL_HANDLE && s_ContextCount == 0)
            {
                vkDestroyDevice(s_Device, nullptr);

                if (s_EnableValidationLayers)
                {
                    DestroyDebugUtilsMessengerEXT(s_Instance, s_DebugMessenger, nullptr);
                }

                vkDestroyInstance(s_Instance, nullptr);
                s_Instance = VK_NULL_HANDLE;
            }
        }

        void UseContext() override
        {
        }


        void SetViewport(uint32_t width, uint32_t height) override
        {
            m_Width = width;
            m_Height = height;
        }

        static VkDevice GetDevice() { return s_Device; }

        Ref<Framebuffer> GetFramebuffer() override
        {
            return nullptr;
        }

        uint32_t GetVSyncInterval() override { return m_SwapInterval; }

        void SetVSync(uint32_t interval) override
        {
            m_SwapInterval = interval;
        }

        void SwapBuffers() override
        {
        }

        void BindDefaultFramebuffer() override
        {
        }

        bool Init() override
        {
            HL_EXCEPTION(s_EnableValidationLayers && !checkValidationLayerSupport(), "Validation layers requested, but not available!");

            if (s_Instance == VK_NULL_HANDLE)
            {

#pragma region Check extensions
                uint32_t extensionCount = 0;
                vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

                std::vector<VkExtensionProperties> extensionsTmp(extensionCount);
                vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsTmp.data());

                std::cout << "\nVulkan: Available extensions:\n";

                for (const auto &extension : extensionsTmp)
                {
                    std::cout << '\t' << extension.extensionName << std::endl;
                }

                std::cout << std::endl;
#pragma endregion

                VkApplicationInfo appInfo{};
                appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                appInfo.pApplicationName = Application::GetInstance().m_applicationName.c_str();
                appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
                appInfo.pEngineName = "HeliosEngine";
                appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
                appInfo.apiVersion = VK_API_VERSION_1_0;

                VkInstanceCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                createInfo.pApplicationInfo = &appInfo;

                auto extensions = getRequiredExtensions();

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

                HL_EXCEPTION(vkCreateInstance(&createInfo, nullptr, &s_Instance) != VK_SUCCESS, "Failed to create Vulkan Instance!");

                std::cout << "\nVulkan: Instance created successfully!\n"
                          << std::endl;

                createSurface();
                pickPhysicalDevice();
                createLogicalDevice();
                createSwapChain();
            } else {
                createSurface();
                createSwapChain();
            }

            s_ContextCount++;
            return true;
        }

    private:

        

        void createLogicalDevice()
        {
            QueueFamilyIndices indices = findQueueFamilies(s_PhysicalDevice);

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

            createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceExtensions.size());

            if (s_EnableValidationLayers)
            {
                createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
                createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
            } else createInfo.enabledLayerCount = 0;

            HL_EXCEPTION(vkCreateDevice(s_PhysicalDevice, &createInfo, nullptr, &s_Device) != VK_SUCCESS, "Failed to create logical device!")

            vkGetDeviceQueue(s_Device, indices.graphicsFamily.value(), 0, &s_GraphicsQuueue);
            vkGetDeviceQueue(s_Device, indices.presentFamily.value(), 0, &s_PresentQueue);
        }

        void createSwapChain()
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(s_PhysicalDevice);

            VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
            VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
            VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

            uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
            if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            {
                imageCount = swapChainSupport.capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = s_Surface;

			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = surfaceFormat.format;
			createInfo.imageColorSpace = surfaceFormat.colorSpace;
			createInfo.imageExtent = extent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			QueueFamilyIndices indices = findQueueFamilies(s_PhysicalDevice);
			uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

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
			createInfo.presentMode = presentMode;
			createInfo.clipped = VK_TRUE;

			createInfo.oldSwapchain = VK_NULL_HANDLE;

			//HL_EXCEPTION(vkCreateSwapchainKHR(s_Device, &createInfo, nullptr, &s_swapchain) != VK_SUCCESS, "Failed to create swap chain!");
            
            VkResult result;
            
            if ((result = vkCreateSwapchainKHR(s_Device, &createInfo, nullptr, &s_swapchain)) != VK_SUCCESS)
            {
				throw std::runtime_error("Failed to create swap chain!" + std::to_string(result));
            }

			vkGetSwapchainImagesKHR(s_Device, s_swapchain, &imageCount, nullptr);
            s_SwapchainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(s_Device, s_swapchain, &imageCount, s_SwapchainImages.data());

			s_SwapchainImageFormat = surfaceFormat.format;
			s_SwapchainExtent = extent;
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

        struct SwapChainSupportDetails
        {
             VkSurfaceCapabilitiesKHR capabilities;
             std::vector<VkSurfaceFormatKHR> formats;
			 std::vector<VkPresentModeKHR> presentModes;
        };

        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device)
        {
            SwapChainSupportDetails details;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, s_Surface, &details.capabilities);

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, s_Surface, &formatCount, nullptr);

            if (formatCount != 0)
            {
                details.formats.resize(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, s_Surface, &formatCount, details.formats.data());
            }

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, s_Surface, &presentModeCount, nullptr);

            if (presentModeCount != 0)
            {
                details.presentModes.resize(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, s_Surface, &presentModeCount, nullptr);
            }
            return details;
        }

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
        {
            if (capabilities.currentExtent.width != UINT32_MAX)
            {
                return capabilities.currentExtent;
            }
            else
            {
                // Get current window size
                VkExtent2D actualExtent =
                {
                    static_cast<uint32_t>(m_Width),
                    static_cast<uint32_t>(m_Height)
                };

                actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }

        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
        {
            for (const auto& availablePresentMode : availablePresentModes)
            {
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    return availablePresentMode;
                }
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
        {
            for (const auto& availableFormat : availableFormats)
            {
                if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return availableFormat;
                }
            }

            return availableFormats[0];
        }

        void pickPhysicalDevice()
        {
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(s_Instance, &deviceCount, nullptr);

            HL_EXCEPTION(deviceCount == 0, "Failed to find GPUs with Vulkan support!");

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(s_Instance, &deviceCount, devices.data());

            for (const auto &device : devices)
            {
                if (isDeviceSuitable(device))
                {
                    s_PhysicalDevice = device;
                    break;
                }
            }

            HL_EXCEPTION(s_PhysicalDevice == VK_NULL_HANDLE, "Failed to find a suitable GPU!");
        }

        bool isDeviceSuitable(VkPhysicalDevice device)
        {
            QueueFamilyIndices indecies = findQueueFamilies(device);

            return indecies.IsCompleted();
        }

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
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
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, s_Surface, &presentSupport);

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

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData)
        {

            std::cerr << "Vulkan: Validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }

        std::vector<const char *> getRequiredExtensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char **glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

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

            for (const char *layerName : s_ValidationLayers)
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

        void setupDebugMessenger()
        {
            if (!s_EnableValidationLayers)
                return;

            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            populateDebugMessengerCreateInfo(createInfo);

            HL_EXCEPTION(CreateDebugUtilsMessengerEXT(s_Instance, &createInfo, nullptr, &s_DebugMessenger) != VK_SUCCESS, "Failed to set up debug messenger!");
        }

        void createSurface()
        {
            HL_EXCEPTION(glfwCreateWindowSurface(s_Instance, (GLFWwindow*)Application::GetInstance().GetWindow()->GetNativeWindow(), nullptr, &s_Surface) != VK_SUCCESS, "Failed to create window surface!");
        }

    private:
        inline static VkDevice s_Device = VK_NULL_HANDLE;
        inline static VkQueue s_GraphicsQuueue = VK_NULL_HANDLE;

        inline static VkInstance s_Instance = VK_NULL_HANDLE;
        inline static VkDebugUtilsMessengerEXT s_DebugMessenger = VK_NULL_HANDLE;
        inline static VkPhysicalDevice s_PhysicalDevice = VK_NULL_HANDLE;

        VkSurfaceKHR s_Surface = VK_NULL_HANDLE;
        VkQueue s_PresentQueue = VK_NULL_HANDLE;

        VkSwapchainKHR s_swapchain;
        std::vector<VkImage> s_SwapchainImages;
        VkFormat s_SwapchainImageFormat;
		VkExtent2D s_SwapchainExtent;

        uint32_t m_SwapInterval = 0u;
        uint32_t m_Width, m_Height;

        inline static uint32_t s_ContextCount = 0;
#ifdef _DEBUG
        // inline static bool s_EnableValidationLayers = true; broken rn
        inline static bool s_EnableValidationLayers = true;
#else
        inline static bool s_EnableValidationLayers = false;
#endif

        inline static const std::vector<const char *> s_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        inline static const std::vector<const char *> s_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
    };
}