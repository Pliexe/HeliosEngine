#pragma once

#include "Helios/Graphics/GPURenderPass.h"
#include "Helios/Graphics/NativeCommandList.h"
#include "Helios/Resources/Buffer.h"
#include "Platform/Vulkan/Buffers/VkIndexBuffer.h"
#include "Platform/Vulkan/Sync/VkSyncableResource.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkImage.h"
#include "Platform/Vulkan/VkRenderPassCache.h"
#include <initializer_list>
#include <optional>
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{

struct VkCommandBeginPass
{
    RenderPassBeginInfo beginInfo;
    std::vector<VkClearValue> clearValues;
    // VkRenderPassCacheKey renderPassCacheKey;
    // ::VkImageView attachments[MAX_ATTACHMENTS];
    // std::size_t attachmentCount = 0;

    // std::uint32_t width;
    // std::uint32_t height;
};

struct VkCommandEndPass
{
};

struct VkCommandBindPipeline
{
    std::vector<Ref<ShaderModule>> vertexShaderModules;
    std::vector<Ref<ShaderModule>> pixelShaderModules;
    std::vector<Ref<ShaderModule>> geometryShaderModules;

    Ref<VertexShader> vertexShader;
    Ref<PixelShader> pixelShader;
    Ref<GeometryShader> geometryShader;

    Topology topology = Topology::TriangleList;

    CullMode cullMode = CullMode::Back;
    FillMode fillMode = FillMode::Solid;
};

struct VkCommandBindVertexBuffers
{
    std::vector<VertexBufferBindInfo> buffers;
};

struct VkCommandBindIndexBuffer
{
    Ref<VkIndexBuffer> buffer;
    VkDeviceSize offset;
};

struct VkCommandDraw
{
    std::uint32_t vertexCount = 0u;
    std::uint32_t instanceCount = 0u;
    std::uint32_t firstVertex = 0u;
    std::uint32_t firstInstance = 0u;
};

struct VkCommandDrawIndexed
{
    std::uint32_t indexCount = 0u;
    std::uint32_t instanceCount = 0u;
    std::uint32_t firstIndex = 0u;
    std::uint32_t firstInstance = 0u;
    std::int32_t vertexOffset = 0;
};

struct VkCommandBindUniformBuffer
{
    const Ref<ShaderModule> module;
    const std::uint32_t slot;
    const Ref<UnsafeUniformBuffer> buffer;
};


struct VkCommandBindImage
{
    const Ref<ShaderModule> module;
    const std::uint32_t slot;
    const Ref<Image> buffer;
};

struct VkCommandBindSampler
{
    const Ref<ShaderModule> module;
    const std::uint32_t slot;
    const Ref<Sampler> sampler;
};

struct VkCommandBindLayouts
{
};

using AllVkCommands =
    std::variant<VkCommandBeginPass, VkCommandEndPass, VkCommandBindPipeline,
                 VkCommandBindVertexBuffers, VkCommandBindIndexBuffer,
                 VkCommandBindUniformBuffer,
                 VkCommandBindImage,
                 VkCommandBindSampler, VkCommandBindLayouts, VkCommandDraw,
                 VkCommandDrawIndexed>;

class VkCommandList final : public NativeCommandList, public std::enable_shared_from_this<VkCommandList>
{
  public:
    VkCommandList(const Ref<VkGraphicsDevice>& device,
                  VkCommandPool commandPool, bool isTransient);
    ~VkCommandList();


    VkCommandBuffer GetCommandBuffer() const { return m_commandBuffer; }
    SyncTicket GetLastTicket() const { return m_lastTicket; }

    void Reset() override;
    void Compile() override;
    void Execute() override;

    void BeginRenderPass(const RenderPassBeginInfo& renderPassInfo) override;
    void EndRenderPass() override;

    void BindPipeline(const BindPipelineInfo& pipelineInfo) override;

    void BindUniformBuffer(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<UnsafeUniformBuffer>& buffer) override;
    void BindTexture2D(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<Texture2D>& buffer) override;
    void BindImage(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<Image>& buffer) override;
    void BindSampler(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<Sampler>& sampler) override;

    void BindUniformBuffer(std::string_view name, const Ref<UnsafeUniformBuffer>& buffer) override;
    void BindTexture2D(std::string_view name, const Ref<Texture2D>& buffer) override;
    void BindImage(std::string_view name, const Ref<Image>& buffer) override;
    void BindSampler(std::string_view name, const Ref<Sampler>& sampler) override;

    void BindVertexBuffers(std::initializer_list<Ref<UnsafeVertexBuffer>> buffers) override;
    void BindVertexBuffers(std::initializer_list<VertexBufferBindInfo> buffers) override;
    void BindIndexBuffer(const Ref<IndexBuffer>& buffer, std::uint32_t offset) override;

    void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) override;
    void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex = 0u, std::int32_t vertexOffset = 0, std::uint32_t firstInstance = 0u) override;

    void TransitionLayout(Ref<VkImage> image, VkImageLayout oldLayout, VkImageLayout newLayout);

    void AppendCommand();

  private:
    friend class VkContext;
    friend class VkCommandList;
    friend class VkGraphicsDevice;

    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    bool m_recorded = false;
    bool m_isTransient = false;
    Ref<VkGraphicsDevice> m_device;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    std::vector<AllVkCommands> m_commands;
    std::unordered_set<VkSemaphore> m_waitSemaphores;
    std::unordered_map<Ref<VkImage>, VkImageLayout> m_expectedInitialLayouts;

    std::optional<std::size_t> m_lastBoundPipelineIndex;
    std::optional<InputLayouts> m_lastBoundPipelineInputLayouts;

    Ref<IndexBuffer> currentIndexBuffer;
    std::uint32_t currentIndexBufferOffset = 0u;

    SyncTicket m_lastTicket;

    VkResourceSyncList m_resourceSync;

    uint32_t m_vbValidMask = 0;
    std::array<std::optional<InputLayout>, 32> m_boundVbLayouts;
    bool m_in_renderPass = false;
};
} // namespace Helios::Internal