#pragma once

#include "Helios/Resources/Shader.h"
#include "Platform/Vulkan/VkRenderPassCache.h"
#include "pch.h"

namespace Helios::Internal
{
    class VkShaderCache
    {
    public:
        static Ref<VkShaderCache> Create();

        template <class T = Shader>
        Ref<T> Acquire(const std::vector<Ref<ShaderModule>>& modules)
        {
            ShaderModule::Stage stage;

            if constexpr (std::is_same_v<T, VertexShader>) stage = ShaderModule::Stage::Vertex;
            else if constexpr (std::is_same_v<T, PixelShader>) stage = ShaderModule::Stage::Pixel;
            else if constexpr (std::is_same_v<T, GeometryShader>) stage = ShaderModule::Stage::Geometry;
            else if constexpr (std::is_same_v<T, ComputeShader>) stage = ShaderModule::Stage::Compute;
            else static_assert(std::is_same_v<T, T>, "Invalid shader type");

            const ShaderKey key = ShaderKey(stage, modules);

            auto valPtr = m_shaderCache.Get(key);
            if (valPtr != nullptr) {
                 if (auto cached = valPtr->lock())
                    return std::dynamic_pointer_cast<T>(cached);
                m_shaderCache.Erase(key);
            }

            auto shader = T::Create(modules);
            
            m_shaderCache.Insert(key, shader);
            return shader;
        }

        std::size_t GetShaderCacheSize() const { return m_shaderCache.Size(); }

        void Step() { m_shaderCache.Step(); }

    private:
        static constexpr bool TICK_BASED = true;
        static constexpr std::uint32_t SHADER_DECAY_THRESHOLD = 60 * 4;

        DecayingMapCache<ShaderKey, WeakRef<Shader>, SHADER_DECAY_THRESHOLD, TICK_BASED> m_shaderCache;

    };
}