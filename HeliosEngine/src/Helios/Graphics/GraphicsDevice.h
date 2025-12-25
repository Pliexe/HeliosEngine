#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Resources/Buffer.h"
#include "Helios/Resources/Image.h"
#include "Helios/Resources/Shader.h"
#include "Helios/Resources/Texture.h"
#include "Helios/Graphics/Sampler.h"
#include "Helios/Graphics/CommandAllocator.h"

namespace Helios
{
    class HELIOS_API GraphicsDevice
    {
    public:

        GraphicsDevice();
		virtual ~GraphicsDevice() = default;
        
        Ref<UnsafeUniformBuffer> GetDummyUniformBuffer();
        Ref<Texture2D> GetDummyTexture2D();
        Ref<Sampler> GetDummySampler();
        
        virtual CommandAllocator& GetAllocatorForThread() = 0;
        
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;

        enum class State
        {
            None,
            Initializing,
            Initialized,
            Terminated,
            Terminating,
            Lost,
        };

        State GetState() { return m_state; }
        std::string_view GetStateName();

        virtual Ref<UnsafeUniformBuffer>
        CreateUniformBuffer(std::uint32_t size) = 0;

        virtual Ref<UnsafeVertexBuffer>
        CreateVertexBuffer(const void* data, uint32_t size,
                           BufferUsage usage = BufferUsage::Static) = 0;
        virtual Ref<IndexBuffer>
        CreateIndexBuffer(uint32_t* indices, uint32_t count,
                          BufferUsage usage = BufferUsage::Static) = 0;
        
        virtual Ref<Texture2D> CreateTexture2D(const std::filesystem::path& path) = 0;
        virtual Ref<Texture2D> CreateTexture2D(uint32_t width, uint32_t height) = 0;
        
        virtual Ref<VertexShader> CreateVertexShader(const std::vector<Ref<ShaderModule>>& shaderModules) = 0;
        virtual Ref<VertexShader> CreateVertexShader(const std::filesystem::path &path, InputLayouts inputLayouts) = 0;
        
        virtual Ref<PixelShader> CreatePixelShader(const std::vector<Ref<ShaderModule>>& shaderModules) = 0;
        virtual Ref<PixelShader> CreatePixelShader(const std::filesystem::path &path) = 0;
        
        virtual Ref<GeometryShader> CreateGeometryShader(const std::vector<Ref<ShaderModule>>& shaderModules) = 0;
        virtual Ref<GeometryShader> CreateGeometryShader(const std::filesystem::path &path) = 0;
        
        virtual Ref<Image> CreateImage(Image::Usage usage, Image::Format format, std::uint32_t width, std::uint32_t height) = 0;
        
        
        friend class Graphics;
        
    protected:
        virtual void Begin() = 0;
        virtual void End() = 0;

        void UpdateState(State newState);

      private:
        friend class Graphics;
        friend class VkGraphicsDevice;

        State m_state = State::None;

        struct Impl;
        Ref<Impl> m_impl;
    };
}