#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Core/Application.h"
#include "Helios/Graphics/GraphicsDevice.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkRenderPassCache.h"
#include "Platform/Vulkan/VkFramebufferCache.h"
#include "Platform/Vulkan/GraphicsPipeline/VkGraphicsPipelineCache.h"
#include "Platform/Vulkan/GraphicsPipeline/VKDescriptorCache.h"

#include <imgui.h>
#include <string>

#include "Helios/Graphics/Graphics.h"

namespace Helios::Debug
{
    class CacheDebugger
    {
    public:
        static void OnImGuiRender()
        {
            ImGui::Begin("Cache Debugger");

            // This is a bit of a hack, assuming we are running Vulkan and casting directly.
            // In a proper engine structure, this should be exposed via generic interfaces or a dedicated debug system.
            auto vkDevice = std::dynamic_pointer_cast<Internal::VkGraphicsDevice>(Graphics::GetMainDevice());

            if (vkDevice)
            {
                ImGui::Text("Vulkan Caches:");
                ImGui::Separator();

                ImGui::Text("RenderPass Cache: %zu", vkDevice->GetRenderPassCache().GetSize());
                ImGui::Text("Framebuffer Cache: %zu", vkDevice->GetFramebufferCache().GetSize());
                
                ImGui::Separator();
                ImGui::Text("Graphics Pipeline Cache: %zu", vkDevice->GetGraphicsPipelineCache().GetPipelineCacheSize());
                ImGui::Text("Shader Cache: %zu", vkDevice->GetShaderCache().GetShaderCacheSize());

                ImGui::Separator();
                ImGui::Text("Static Descriptor Set Cache: %zu", vkDevice->GetDescriptorSetCache().GetStaticCacheSize());
                ImGui::Text("Transient Descriptor Set Cache: %zu", vkDevice->GetDescriptorSetCache().GetTransientCacheSize());

                ImGui::Separator();
                ImGui::Text("Sync Pool:");
                ImGui::Text("   Size: %zu", vkDevice->GetSyncPool().GetSize());
                ImGui::Text("   Recycle Count: %zu", vkDevice->GetSyncPool().GetRecycleCount());
                ImGui::Text("   In Use: %zu", vkDevice->GetSyncPool().GetSize() - vkDevice->GetSyncPool().GetRecycleCount());
                ImGui::Text("   Capacity: %zu", vkDevice->GetSyncPool().GetCapacity());

                ImGui::Separator();
                bool enableStats = vkDevice->GetSyncPool().IsStatsEnabled();
                if (ImGui::Checkbox("Enable Sync Pool Stats Logging", &enableStats))
                    vkDevice->GetSyncPool().SetStatsEnabled(enableStats);

                if (enableStats)
                {
                    ImGui::TextUnformatted(vkDevice->GetSyncPool().GetStatsString().c_str());
                }
            }
            else
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Graphics Device is not Vulkan or null!");
            }

            ImGui::End();
        }
    };
}
