#include "VkImage.h"

#include "Helios/Core/Exceptions.h"
#include "Helios/Resources/Image.h"
#include "Platform/Vulkan/Sync/VkSyncPool.h"

#include "Platform/Vulkan/VkSampler.h"
#include "Platform/Vulkan/VkUtils.h"
#include "Platform/Vulkan/VkWrappers.h"

#include "imgui_impl_vulkan.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{
    static VkFormat GetVkFormat(Image::Format format)
    {
        using Format = Image::Format;
        switch (format)
        {
        // 32-bit float formats
        case Format::R32G32B32A32_FLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Format::R32G32B32_FLOAT:     return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R32G32_FLOAT:        return VK_FORMAT_R32G32_SFLOAT;
        case Format::R32_FLOAT:           return VK_FORMAT_R32_SFLOAT;

        // 32-bit signed int formats
        case Format::R32G32B32A32_SINT:  return VK_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32B32_SINT:      return VK_FORMAT_R32G32B32_SINT;
        case Format::R32G32_SINT:         return VK_FORMAT_R32G32_SINT;
        case Format::R32_SINT:            return VK_FORMAT_R32_SINT;

        // 32-bit unsigned int formats
        case Format::R32G32B32A32_UINT:  return VK_FORMAT_R32G32B32A32_UINT;
        case Format::R32G32B32_UINT:      return VK_FORMAT_R32G32B32_UINT;
        case Format::R32G32_UINT:         return VK_FORMAT_R32G32_UINT;
        case Format::R32_UINT:            return VK_FORMAT_R32_UINT;

        // 16-bit float formats
        case Format::R16G16B16A16_FLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case Format::R16G16_FLOAT:        return VK_FORMAT_R16G16_SFLOAT;
        case Format::R16_FLOAT:           return VK_FORMAT_R16_SFLOAT;

        // 16-bit unsigned normalized formats
        case Format::R16G16B16A16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
        case Format::R16G16_UNORM:        return VK_FORMAT_R16G16_UNORM;
        case Format::R16_UNORM:           return VK_FORMAT_R16_UNORM;

        // 8-bit unsigned normalized formats
        case Format::B8G8R8A8_UNORM:      return VK_FORMAT_B8G8R8A8_UNORM;
        case Format::R8G8B8A8_UNORM:      return VK_FORMAT_R8G8B8A8_UNORM;
        // case Format::R8G8B8_UNORM:     // Not commonly supported, omit or add if needed
        case Format::R8G8_UNORM:          return VK_FORMAT_R8G8_UNORM;
        case Format::R8_UNORM:            return VK_FORMAT_R8_UNORM;

        // Depth and stencil
        case Format::D24_UNORM_S8_UINT:   return VK_FORMAT_D24_UNORM_S8_UINT;
        case Format::D32_FLOAT:           return VK_FORMAT_D32_SFLOAT;

        case Format::None:
        default:
            HL_ASSERT(false, "Unknown format!");
            return VK_FORMAT_UNDEFINED;
        }
    }

    VkImage::VkImage(const Ref<VkGraphicsDevice>& device, Usage usage, Format format, std::uint32_t width, std::uint32_t height)
        : BaseVkSyncableResource(device)
    {
        HL_EXCEPTION((usage & Usage::Swapchain) == Usage::Swapchain, "You can't create a image of type Swapchain. They are owned by GPU/OS!");
        HL_EXCEPTION(width <= 0 || height <= 0, "Image width and height must be greater than 0!");

        m_usage = usage;
        m_width = width;
        m_height = height;
        m_format = format;

        if (IsDepth()) 
            m_defaultLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        else if ((m_usage & Usage::Sampled) == Usage::Sampled) 
            m_defaultLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        else if ((m_usage & Usage::Storage) == Usage::Storage) 
            m_defaultLayout = VK_IMAGE_LAYOUT_GENERAL;
        else if ((m_usage & Usage::ColorAttachment) == Usage::ColorAttachment) 
            m_defaultLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        else if ((m_usage & Usage::TransferDst) == Usage::TransferDst) 
            m_defaultLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        else if ((m_usage & Usage::TransferSrc) == Usage::TransferSrc) 
            m_defaultLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        else 
            m_defaultLayout = VK_IMAGE_LAYOUT_GENERAL;

        Invalidate();
    }

    VkImage::VkImage(const Ref<VkGraphicsDevice>& device, ::VkImage swapchainImage, VkFormat swapchainFormat, std::uint32_t width, std::uint32_t height)
        : BaseVkSyncableResource(device)
    {
        HL_EXCEPTION(width <= 0 || height <= 0, "Image width and height must be greater than 0!");

        m_device = device;
        m_swapchainFormat = swapchainFormat;
        m_width = width;
        m_height = height;
        m_format = Format::None;
        m_usage = Usage::Swapchain | Usage::ColorAttachment;
        m_image = Vulkan::VkImageWrapper(VK_NULL_HANDLE, swapchainImage);
        m_defaultLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        // Set device to VK_NULL_HANDLE to avoid vkDestroy since it's a swapchain image

        CreateSwapchainImageView();
    }
    

    VkImage::~VkImage()
    {
        Cleanup();
    }

    void VkImage::Resize(std::uint32_t width, std::uint32_t height)
    {
        HL_EXCEPTION(IsSwapchain(), "You can't resize a image of type Swapchain. They are owned by GPU/OS!");
        m_width = width;
        m_height = height;
        Invalidate();
    }

    void transitionImageLayout(const Ref<VkGraphicsDevice>& device, ::VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {        
        VkCommandBuffer commandBuffer = device->LegacyBeginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        // vkCmdPipelineBarrier(
        //     commandBuffer,
        //     0, 0,
        //     0,
        //     0, nullptr,
        //     0, nullptr,
        //     1, &barrier
        // );

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            // Generic fallback
            barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        device->LegacyEndSingleTimeCommands(commandBuffer);
    }

    size_t VkImage::CalculateSize()
    {
        size_t formatSize = 0;

        auto test = sizeof(int32) * 4;

        switch (m_format)
        {
            case Format::None: HL_EXCEPTION(true, "Format is None!");
            case Format::R8G8B8A8_UNORM:
                formatSize = 4;     // uint8 * 4
                break;
            case Image::Format::R32G32B32A32_FLOAT:
                formatSize = 4 * 4; // float * 4
                break;
            case Image::Format::R32G32B32_FLOAT:
                formatSize = 4 * 3; // float * 3
                break;
            case Image::Format::R32G32_FLOAT:
                formatSize = 4 * 2; // float * 2
                break;
            case Image::Format::R32_FLOAT:
                formatSize = 4;     // float * 1
                break;
            case Image::Format::R32G32B32A32_SINT:
                formatSize = 4 * 4; // int32 * 4
                break;
            case Image::Format::R32G32B32_SINT:
                formatSize = 4 * 3; // int32 * 3
                break;
            case Image::Format::R32G32_SINT:
                formatSize = 4 * 2; // int32 * 2
                break;
            case Image::Format::R32_SINT:
                formatSize = 4;     // int32 * 1
                break;
            case Image::Format::R32G32B32A32_UINT:
                formatSize = 4 * 4; // uint32 * 4
                break;
            case Image::Format::R32G32B32_UINT:
                formatSize = 4 * 3; // uint32 * 3
                break;
            case Image::Format::R32G32_UINT:
                formatSize = 4 * 2; // uint32 * 2
                break;
            case Image::Format::R32_UINT:
                formatSize = 4;     // uint32 * 1
                break;
            case Image::Format::R16G16B16A16_FLOAT:
                formatSize = 2 * 4; // float16 * 4
                break;
            case Image::Format::R16G16_FLOAT:
                formatSize = 2 * 2; // float16 * 2
                break;
            case Image::Format::R16_FLOAT:
                formatSize = 2;     // float16 * 1
                break;
            case Image::Format::R16G16B16A16_UNORM:
                formatSize = 2 * 4; // uint16 * 4
                break;
            case Image::Format::R16G16_UNORM:
                formatSize = 2 * 2; // uint16 * 2
                break;
            case Image::Format::R16_UNORM:
                formatSize = 2;     // uint16 * 1
                break;
            case Image::Format::B8G8R8A8_UNORM:
                formatSize = 1 * 4; // uint8 * 4
                break;
            case Image::Format::R8G8_UNORM:
                formatSize = 1 * 2; // uint8 * 2
                break;
            case Image::Format::R8_UNORM:
                formatSize = 1;     // uint8 * 1
                break;
            case Image::Format::D24_UNORM_S8_UINT:
                formatSize = 4;     // __
                break;
            case Image::Format::D32_FLOAT:
                formatSize = 4;     // float * 1
                break;
            }

        return m_width * m_height * formatSize;
    }


    void VkImage::SetData(void* data, uint32 size)
    {
        HL_EXCEPTION(IsSwapchain(), "You can't set data to a image of type Swapchain. They are owned by GPU/OS!");
        HL_EXCEPTION((m_usage & Usage::TransferDst) != Usage::TransferDst, "Image is not transferable!");
        HL_EXCEPTION(size != CalculateSize(), "Size of data is larger than image size! {} != {} (0x{:x})", size, CalculateSize(), (std::intptr_t)(::VkImage)m_image.Get());

        Wait();
        
        Vulkan::VkBufferWrapper stagingBuffer;
        Vulkan::VkDeviceMemoryWrapper stagingBufferMemory;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        {
            auto result = Vulkan::VkBufferWrapper::Create(*m_device, &bufferInfo, nullptr);
            HL_EXCEPTION(!result, std::format("Failed to create staging buffer! {}", result.error()));
            stagingBuffer = std::move(result.value());
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(*m_device, stagingBuffer.Get(), &memoryRequirements);

        VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = Utils::FindMemoryType(memoryRequirements.memoryTypeBits, properties, m_device->GetNativePhysicalDevice());

        {
            auto result = Vulkan::VkDeviceMemoryWrapper::Create(*m_device, &allocInfo, nullptr);
            HL_EXCEPTION(!result, std::format("Failed to create staging buffer memory! {}", result.error()));
            stagingBufferMemory = std::move(result.value());
        }

        vkBindBufferMemory(*m_device, stagingBuffer.Get(), stagingBufferMemory.Get(), 0);

        void* mapped;
        vkMapMemory(*m_device, stagingBufferMemory.Get(), 0, memoryRequirements.size, 0, &mapped);
        std::memcpy(mapped, data, size);
        vkUnmapMemory(*m_device, stagingBufferMemory.Get());

        transitionImageLayout(m_device, m_image.Get(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {m_width, m_height, 1};
        
        VkCommandBuffer commandBuffer = m_device->LegacyBeginSingleTimeCommands();

        vkCmdCopyBufferToImage(
            commandBuffer,
            stagingBuffer.Get(),
            m_image.Get(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        m_device->LegacyEndSingleTimeCommands(commandBuffer);

        transitionImageLayout(m_device, m_image.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_defaultLayout);
        m_pendingLayout = m_defaultLayout;

        stagingBuffer.Reset();
        stagingBufferMemory.Reset();
    }

    void VkImage::Cleanup()
    {
        {
            std::lock_guard<std::mutex> lock(m_device->m_graphicsQueueMutex);
            vkDeviceWaitIdle(*m_device);
        }        
        m_id.Reset();
        m_imageView.Reset();
        m_image.Reset();
        m_imageMemory.Reset();
    }

    void* VkImage::GetID()
    {
        HL_EXCEPTION((m_usage & Usage::Sampled) != Usage::Sampled, "Image is not sampleable!");

        if (m_sampler == nullptr)
        {                
            m_sampler = std::static_pointer_cast<VkSampler>(Sampler::GetOrCreate(SamplerConfiguration()));
        }

        if (m_id == VK_NULL_HANDLE)
        {                
            HL_ASSERT(m_imageView.Get() != VK_NULL_HANDLE, "Image view is null");
            m_id = Vulkan::ImGuiTextureIDWrapper(VK_NULL_HANDLE, ImGui_ImplVulkan_AddTexture(m_sampler->GetNative(), m_imageView.Get(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        }
        return m_id.Get();
    }

    void VkImage::Invalidate()
    {
        Cleanup();
        Create();
    }
    
    void VkImage::Create()
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_width;
        imageInfo.extent.height = m_height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = GetVkFormat(m_format);
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // Map Image::Usage to VkImageUsageFlags
        imageInfo.usage = 0;
        if ((m_usage & Usage::Sampled) == Usage::Sampled) imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if ((m_usage & Usage::Storage) == Usage::Storage) imageInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
        if ((m_usage & Usage::ColorAttachment) == Usage::ColorAttachment) imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if ((m_usage & Usage::DepthStencil) == Usage::DepthStencil) imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        if ((m_usage & Usage::TransferSrc) == Usage::TransferSrc) imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if ((m_usage & Usage::TransferDst) == Usage::TransferDst) imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;

        auto result = Vulkan::VkImageWrapper::Create(*m_device, &imageInfo, nullptr);
        HL_EXCEPTION(!result.has_value(), "Failed to create image!");
        m_image = std::move(result.value());

        VkMemoryRequirements memReq;
        vkGetImageMemoryRequirements(*m_device, m_image.Get(), &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = Utils::FindMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_device->GetNativePhysicalDevice());
        auto allocResult = Vulkan::VkDeviceMemoryWrapper::Create(*m_device, &allocInfo, nullptr);
        HL_EXCEPTION(!allocResult.has_value(), "Failed to allocate image memory!");
        m_imageMemory = std::move(allocResult.value());

        vkBindImageMemory(*m_device, m_image.Get(), m_imageMemory.Get(), 0);

        VkImageViewCreateInfo imageViewInfo{};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image = m_image.Get();
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = GetVkFormat(m_format);
        imageViewInfo.subresourceRange.aspectMask = IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;

        auto viewResult = Vulkan::VkImageViewWrapper::Create(*m_device, &imageViewInfo, nullptr);
        HL_EXCEPTION(!viewResult.has_value(), "Failed to create image view!");
        m_imageView = std::move(viewResult.value());

        if (m_defaultLayout != VK_IMAGE_LAYOUT_UNDEFINED)
        {
            transitionImageLayout(m_device, m_image.Get(), VK_IMAGE_LAYOUT_UNDEFINED, m_defaultLayout);
            m_pendingLayout = m_defaultLayout;
        }
    }

    void VkImage::CreateSwapchainImageView()
    {
        VkImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = m_image.Get();
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = m_swapchainFormat;
        viewCreateInfo.components = { 
            .r = VK_COMPONENT_SWIZZLE_IDENTITY, .g = VK_COMPONENT_SWIZZLE_IDENTITY, 
            .b = VK_COMPONENT_SWIZZLE_IDENTITY, .a = VK_COMPONENT_SWIZZLE_IDENTITY
        };
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        auto result = Vulkan::VkImageViewWrapper::Create(*m_device, &viewCreateInfo, nullptr);
        HL_EXCEPTION(!result.has_value(), "Failed to create image view in swapchain!");
        m_imageView = std::move(result.value());
    }

    VkFormat VkImage::GetNativeFormat() { 
        if (IsSwapchain()) return m_swapchainFormat; else return GetVkFormat(m_format);
    }

    VkPipelineStageFlags VkImage::GetStage() const
    {
        if (IsDepth()) return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        if ((m_usage & Usage::ColorAttachment) == Usage::ColorAttachment) return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        if ((m_usage & Usage::Sampled) == Usage::Sampled) return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; // Simplified
        if ((m_usage & Usage::TransferDst) == Usage::TransferDst) return VK_PIPELINE_STAGE_TRANSFER_BIT; // Simplified
        if ((m_usage & Usage::TransferSrc) == Usage::TransferSrc) return VK_PIPELINE_STAGE_TRANSFER_BIT; // Simplified
        
        return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
}