#include "GraphicsDevice.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/Sampler.h"
#include "Helios/Resources/Color.h"
#include <cstddef>

namespace Helios
{
    struct GraphicsDevice::Impl
    {
        Ref<UnsafeUniformBuffer> dummyBuffer;
        Ref<Texture2D> dummyTexture2D;
        Ref<Sampler> dummySampler;
    };

    GraphicsDevice::GraphicsDevice()
    {
        m_impl = CreateRef<GraphicsDevice::Impl>();
    }

    Ref<UnsafeUniformBuffer> GraphicsDevice::GetDummyUniformBuffer()
    {
        if (!m_impl->dummyBuffer)
        {
            m_impl->dummyBuffer = CreateUniformBuffer(MAX_BUFFER_SIZE);
            void* zeroData = calloc(1, m_impl->dummyBuffer->GetSize());
            try {
                m_impl->dummyBuffer->SetData(zeroData);
            } catch(const HeliosException&)
            {
                free(zeroData);
                throw;
            }
            free(zeroData);
        }
        return m_impl->dummyBuffer;
    }
    Ref<Texture2D> GraphicsDevice::GetDummyTexture2D()
    {
        if (!m_impl->dummyTexture2D)
        {
            m_impl->dummyTexture2D = CreateTexture2D(2, 2);
            
            static const ColorU8 MagentaChecker[4] = {
                ColorU8::Magenta, ColorU8::Black,
                ColorU8::Black  , ColorU8::Magenta,
            };

            m_impl->dummyTexture2D->SetData((void*)MagentaChecker, sizeof(MagentaChecker));
        }

        return m_impl->dummyTexture2D;
    }
    Ref<Sampler> GraphicsDevice::GetDummySampler()
    {
        static auto samplerConfig = SamplerConfiguration{
            .minFilter = TextureFilter::Nearest,
            .magFilter = TextureFilter::Nearest,
            .mipFilter = MipFilter::Nearest,
        };
        
        if (!m_impl->dummySampler)
            m_impl->dummySampler = Sampler::GetOrCreate(samplerConfig);

        return m_impl->dummySampler;
    }

    std::string_view to_string(GraphicsDevice::State state)
    {
        using State = GraphicsDevice::State;
        switch (state) {
        case State::None: return "None";
        case State::Initializing: return "Initializing";
        case State::Initialized: return "Initialized";
        case State::Terminated: return "Terminated";
        case State::Terminating: return "Terminating";
        case State::Lost: return "Lost";
        }
    }

    void GraphicsDevice::UpdateState(State newState)
    {
        std::cout << std::format("[Device] State {} -> {}", to_string(m_state), to_string(newState)) << std::endl;
        m_state = newState;
    }
    std::string_view GraphicsDevice::GetStateName()
    {
        return to_string(m_state);
    };
} // namespace Helios