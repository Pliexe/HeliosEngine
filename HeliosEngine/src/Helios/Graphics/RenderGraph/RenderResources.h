#pragma once

#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/GraphicsDevice.h"
#include "Helios/Resources/Buffer.h"
#include "Helios/Utils/DecayingCache.h"
#include "RenderResourceKeys.h"

namespace Helios
{
    class RenderResources
    {
    public:

        RenderResources() = delete;
        RenderResources(const RenderResources&) = delete;
        RenderResources& operator=(const RenderResources&) = delete;

        RenderResources(Ref<GraphicsDevice> device)
        {
            m_device = device;
            m_frames.resize(GetMaxFramesInFlight());
        }

        Ref<Image> RequestImage(uint32 width, uint32 height, uint32 layers, uint32 samples, Image::Format format, Image::Usage usage)
        {
            
            ImageResourceKey key = {
                .width = width,
                .height = height,
                .layers = layers,
                .samples = samples,
                .format = format,
                .usage = usage,
            };
            
            auto& nextFrameImages = m_frames[GetNextFrameIndex()].m_images[key];
            
            Ref<Image> image;
            
            if (!nextFrameImages.empty())
            {
                image = nextFrameImages.pop_front();
                m_frames[GetCurrentFrameIndex()].m_images[key].push_back(image);
                return image;
            }
            
            HL_EXCEPTION(width <= 0 || height <= 0, "Cannot request image with invalid dimensions! Width or height must be greater than 0");
            image = m_device->CreateImage(usage, format, width, height);
            m_frames[GetCurrentFrameIndex()].m_images[key].push_back(image);

            return image;
        }

        template <typename T>
        Ref<UniformBuffer<T>> RequestUniformBuffer()
        {
            uint32 size = sizeof(T);

            auto buffer = RequestUnsafeUniformBuffer(size);

            UniformBuffer<T> uniformBuffer = CreateRef<UniformBuffer<T>>(buffer);
            uniformBuffer.m_buffer = buffer;

            return uniformBuffer;
        }

        Ref<UnsafeUniformBuffer> RequestUnsafeUniformBuffer(uint32 size)
        {
            auto& nextFrameBuffers = m_frames[GetNextFrameIndex()].m_uniformBuffers[size];

            Ref<UnsafeUniformBuffer> buffer;

            if (!nextFrameBuffers.empty())
            {
                buffer = nextFrameBuffers.pop_front();
                m_frames[GetCurrentFrameIndex()].m_uniformBuffers[size].push_back(buffer);
                return buffer;
            }

            buffer = m_device->CreateUniformBuffer(size);
            m_frames[GetCurrentFrameIndex()].m_uniformBuffers[size].push_back(buffer);

            return buffer;
        }

        Ref<UnsafeVertexBuffer> RequestUnsafeVertexBuffer(uint32 size)
        {
            auto& nextFrameBuffers = m_frames[GetNextFrameIndex()].m_vertexBuffers[size];

            Ref<UnsafeVertexBuffer> buffer;

            if (!nextFrameBuffers.empty())
            {
                buffer = nextFrameBuffers.pop_front();
                m_frames[GetCurrentFrameIndex()].m_vertexBuffers[size].push_back(buffer);
                return buffer;
            }

            std::cout << std::format("REQUESTING BUFFER: {}", size) << std::endl;
            buffer = m_device->CreateVertexBuffer(nullptr, size, BufferUsage::Dynamic);
            m_frames[GetCurrentFrameIndex()].m_vertexBuffers[size].push_back(buffer);

            return buffer;
        }

        void Step()
        {
            for (auto& [_, value] : m_frames[GetCurrentFrameIndex()].m_images)
            {
                value.step();
            }
            for (auto& [_, value] : m_frames[GetCurrentFrameIndex()].m_uniformBuffers)
            {
                value.step();
            }
        }

        uint8 GetCurrentFrameIndex() const;
        uint8 GetNextFrameIndex() const;
        uint8 GetMaxFramesInFlight() const;   

    private:

        constexpr static std::uint64_t S_THRESHOLD = 5;
        constexpr static bool S_IS_TICK_BASED = true;

        struct FrameData
        {
            std::unordered_map<ImageResourceKey, DecayingCache<Ref<Image>, S_THRESHOLD, S_IS_TICK_BASED>> m_images;
            std::unordered_map<uint32, DecayingCache<Ref<UnsafeUniformBuffer>, S_THRESHOLD, S_IS_TICK_BASED>> m_uniformBuffers;
            std::unordered_map<uint32, DecayingCache<Ref<UnsafeVertexBuffer>, S_THRESHOLD, S_IS_TICK_BASED>> m_vertexBuffers;
        };

        std::vector<FrameData> m_frames;
        Ref<GraphicsDevice> m_device;
    };
}