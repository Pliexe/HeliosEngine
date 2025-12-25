#pragma once

#include "Helios/Graphics/CommandAllocator.h"
#include "Platform/Vulkan/VkCommandAllocator.h"
#include "Platform/Vulkan/Sync/VkSyncableResource.h"


#include "Helios/Graphics/GraphicsDevice.h"
#include <unordered_map>
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{
    using ValidationLayers = const std::vector<const char *>;
    using DeviceExtensions = const std::vector<const char *>;
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsCompleted()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    
    class VkRenderPassCache;
    class VkFramebufferCache;
    class VkSyncTracker;
    class VkDescriptorSetLayoutCache;
    class VkGraphicsPipelineCache;
    class VkDescriptorSetCache;
    class VkSyncPool;
    class VkContext;
    class VkShaderCache;
    class VkGraphicsDevice final : public GraphicsDevice, public std::enable_shared_from_this<VkGraphicsDevice>
    {
    public:
        VkGraphicsDevice(const VkGraphicsDevice &) = delete;
        VkGraphicsDevice& operator=(const VkGraphicsDevice &) = delete;

        VkGraphicsDevice() = default;
        ~VkGraphicsDevice();

        bool Initialize() override;
        void Shutdown() override;

        CommandAllocator& GetAllocatorForThread() override;

        Ref<UnsafeUniformBuffer>
        CreateUniformBuffer(std::uint32_t size) override;

        Ref<UnsafeVertexBuffer> CreateVertexBuffer(const void *data, uint32_t size, BufferUsage usage = BufferUsage::Static) override;
		Ref<IndexBuffer> CreateIndexBuffer(uint32_t* indices, uint32_t count, BufferUsage usage = BufferUsage::Static) override;

        Ref<Texture2D> CreateTexture2D(const std::filesystem::path &path) override;
        Ref<Texture2D> CreateTexture2D(uint32_t width, uint32_t height) override;

        Ref<VertexShader> CreateVertexShader(const std::vector<Ref<ShaderModule>>& shaderModules) override;
        Ref<VertexShader> CreateVertexShader(const std::filesystem::path &path, InputLayouts inputLayouts) override;
        
        Ref<PixelShader> CreatePixelShader(const std::vector<Ref<ShaderModule>>& shaderModules) override;
        Ref<PixelShader> CreatePixelShader(const std::filesystem::path &path) override;
        
        Ref<GeometryShader> CreateGeometryShader(const std::vector<Ref<ShaderModule>>& shaderModules) override;
        Ref<GeometryShader> CreateGeometryShader(const std::filesystem::path &path) override;

        Ref<Image> CreateImage(Image::Usage usage, Image::Format format, std::uint32_t width, std::uint32_t height) override;


        VkDevice GetNativeDevice() const { return m_device; }
        VkPhysicalDevice GetNativePhysicalDevice() const { return m_physicalDevice; }


        QueueFamilyIndices FindQueueFamilies(VkSurfaceKHR surface);
        uint32_t GetGraphicsQueueFamilyIndex() { return m_graphicsQueueFamilyIndex; }
        const VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const { 
            return m_physicalDevicePropeties;
        } 


        static VkInstance GetVkInstance() { return s_vulkanInstance; }

        VkQueue GetGraphicsQueue() { return m_graphicsQueue; }

        operator VkDevice()
        {
            return m_device;
        }

        VkRenderPassCache& GetRenderPassCache();
        VkFramebufferCache& GetFramebufferCache();
        VkSyncTracker& GetSyncTracker();
        VkDescriptorSetLayoutCache& GetDescriptorSetLayoutCache();
        VkGraphicsPipelineCache& GetGraphicsPipelineCache();
        VkDescriptorSetCache& GetDescriptorSetCache();
        VkSyncPool& GetSyncPool();
        VkShaderCache& GetShaderCache();
        
        VkCommandBuffer LegacyBeginSingleTimeCommands();
        void LegacyEndSingleTimeCommands(VkCommandBuffer cmd);

        VkFence GetSingleUseSubmitFence() { return m_singleUseSubmitFinished; }
        VkSemaphore GetSingleUseSubmitSemaphore() { return m_singleUseSubmitSemaphore; }

        struct SubmitResourceInfo
        {
            std::vector<std::shared_ptr<IVkSyncableResource>> readResources;
            std::vector<std::shared_ptr<IVkSyncableResource>> writeResources;
        };

        struct SubmitInfo
        {
            VkCommandBuffer* commandBuffers;
            uint32_t commandBufferCount;
            
            std::vector<VkSemaphore> waitSemaphores;
            std::vector<VkPipelineStageFlags> waitStages;
            
            VkSemaphore signalSemaphore = VK_NULL_HANDLE;
            VkFence signalFence = VK_NULL_HANDLE;

            std::optional<SyncTicket> dependency;
        };

        SyncTicket Submit(const SubmitInfo& info, const SubmitResourceInfo& resourceInfo);

    private:

        Ref<VkCommandAllocator> m_SingleUseCommandPool;
        std::mutex m_transientPoolMutex;
        std::mutex m_transientCommandListMutex;
        VkFence m_singleUseSubmitFinished = VK_NULL_HANDLE;
        VkSemaphore m_singleUseSubmitSemaphore = VK_NULL_HANDLE;


        void RegisterTransientCommandList(Ref<NativeCommandList> cmdList);

    protected:
        void End() override;
        void Begin() override;
    private:

        friend class VkContext;
        friend class VkSwapChain;
        friend class VkDescriptorSetCache;
        friend class VkCommandList;
        friend class VkCommandListImmediate;
        friend class VkTransitionList;
        friend class ThreadAllocatorGuard;

        friend class VkImage;

        struct Impl;
        Ref<Impl> m_impl;

        std::vector<VkContext*> m_contexts;
        
        std::vector<VkSemaphore> m_cmdQueueSemaphores;
        
        Ref<VkRenderPassCache> m_renderPassCache;
        Ref<VkFramebufferCache> m_framebufferCache;
        Ref<VkSyncTracker> m_syncTracker;
        Ref<VkDescriptorSetLayoutCache> m_descriptorSetLayoutCache;
        Ref<VkGraphicsPipelineCache> m_graphicsPipelineCache;
        Ref<VkDescriptorSetCache> m_descriptorSetCache;
        Ref<VkSyncPool> m_syncPool;
        Ref<VkShaderCache> m_shaderCache;

        std::unordered_map<std::thread::id, VkCommandAllocator> m_allocators;
    
        VkPhysicalDevice m_physicalDevice;
        VkDevice m_device;
        VkQueue m_graphicsQueue;
        uint32_t m_graphicsQueueFamilyIndex = 0;
        VkPhysicalDeviceProperties m_physicalDevicePropeties;
        VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;

        std::mutex m_graphicsQueueMutex;
        std::mutex m_allocatorMutex;
        
        bool is_shutdown = false;
        bool is_initialized = false;
        
        inline static uint32_t s_devicesInUse = 0;
        inline static VkInstance s_vulkanInstance = VK_NULL_HANDLE;
        inline static VkDebugUtilsMessengerEXT s_DebugMessenger = VK_NULL_HANDLE;
    #ifdef HELIOS_DEBUG
        inline static bool s_EnableValidationLayers = true;
    #else
        inline static bool s_EnableValidationLayers = false;
    #endif
        inline static ValidationLayers s_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation",
        };

        inline static DeviceExtensions s_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef HELIOS_DEBUG
            VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
#endif
		};
    };
}

#include "VkGraphicsDevice.inl"