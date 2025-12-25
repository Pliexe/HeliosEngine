#include "Graphics.h"
#include "Helios/Core/Exceptions.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"

#include <cstdint>


namespace Helios
{    
    bool Graphics::Initialize(API api)
    {        
        s_maxFramesInFlight = 3;
        
        switch (api) {
            case API::Vulkan: s_mainDevice = CreateRef<Internal::VkGraphicsDevice>(); break;
            default: return false; 
        }       

        bool success = s_mainDevice->Initialize();

        if (!success) return false;

        s_mainRenderGraph = CreateRef<RenderGraph>(s_mainDevice);

        s_mainCommandLists.reserve(GetMaxFramesInFlight());

        auto& alloc = s_mainDevice->GetAllocatorForThread();
        
        for (std::uint8_t i = 0; i < GetMaxFramesInFlight(); ++i)
            s_mainCommandLists.emplace_back(alloc.CreateCommandList()); // Should be able to auto optimize?
        
        s_api = api;
        return true;
    }

    void Graphics::Shutdown()
    {
        s_mainDevice->Shutdown();
        s_mainDevice.reset();
    }

    void Graphics::Begin()
    {
        s_mainDevice->Begin();

        s_mainCommandLists[GetCurrentFrameIndex()]->Reset();
    }

    std::uint8_t Graphics::GetNextFrameIndex() { return (s_currentFrameIndex + 1) % Graphics::GetMaxFramesInFlight(); }
    // Must only call in main thread in main loop!
    void Graphics::End()
    {
        s_mainCommandLists[GetCurrentFrameIndex()]->Execute();

        HL_EXCEPTION(s_mainDevice == nullptr, "Graphics Device is not initialized!");
        s_mainDevice->End();

        s_currentFrameIndex = GetNextFrameIndex();
        s_frameCount++;
    }

    std::uint8_t Graphics::GetCurrentFrameIndex() { return s_currentFrameIndex; }

    NativeCommandList& Graphics::GetMainCommandList()
    {
        return *s_mainCommandLists[GetCurrentFrameIndex()];
    }

    Ref<RenderGraph> Graphics::GetMainRenderGraph()
    {
        return s_mainRenderGraph;
    }
} // namespace Helios