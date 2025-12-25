#pragma once

#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Graphics/GPURenderPass.h"
#include "Helios/Graphics/Sampler.h"
#include "Helios/Resources/Buffer.h"
#include "Helios/Resources/Material.h"
#include "Helios/Resources/Shader.h"
#include "Helios/Resources/Texture.h"

namespace Helios
{
    enum class Topology
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
    };

    enum class CullMode { None, Front, Back, Both };
    enum class FillMode { Solid, Wireframe };

    struct GraphicsPipelineSpecification
    {
        Ref<VertexShader> vertexShader;
        Ref<PixelShader> pixelShader;
        Ref<GeometryShader> geometryShader;

        Topology topology = Topology::TriangleList;

        CullMode cullMode = CullMode::Back;
        FillMode fillMode = FillMode::Solid;
        // uint32_t sampleCount = 1; // MSAAw
    };
    
    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;

        const BindingLink& GetBindingLink() const { return m_bindingLink; }
    private:
        BindingLink m_bindingLink;
    };

    struct GraphicsPipelineKey
    {
        ShaderKey vertexShaderKey;
        ShaderKey pixelShaderKey;
        std::optional<ShaderKey> geometryShaderKey;

        Topology topology = Topology::TriangleList;

        CullMode cullMode = CullMode::Back;
        FillMode fillMode = FillMode::Solid;

        bool operator==(const GraphicsPipelineKey& other) const {
            return vertexShaderKey == other.vertexShaderKey &&
                pixelShaderKey == other.pixelShaderKey &&
                geometryShaderKey == other.geometryShaderKey &&
                topology == other.topology &&
                cullMode == other.cullMode &&
                fillMode == other.fillMode;
        }
    };
}

namespace std {
	template <>
	struct hash<Helios::GraphicsPipelineKey> {
		size_t operator()(const Helios::GraphicsPipelineKey& key) const {
			size_t h = 0;
			auto combine = [](size_t& seed, size_t val) {
				seed ^= val + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			};

			combine(h, hash<Helios::ShaderKey>{}(key.vertexShaderKey));
			combine(h, hash<Helios::ShaderKey>{}(key.pixelShaderKey));
			if (key.geometryShaderKey) combine(h, hash<Helios::ShaderKey>{}(*key.geometryShaderKey));

			combine(h, static_cast<size_t>(key.topology));
			combine(h, static_cast<size_t>(key.cullMode));
			combine(h, static_cast<size_t>(key.fillMode));
			return h;
		}
	};
}   