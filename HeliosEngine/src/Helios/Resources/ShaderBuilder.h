#pragma once
#include "pch.h"
#include "Shader.h"
#include "Buffer.h"

namespace Helios
{
    class ShaderBuilder
    {
    public:
        ShaderBuilder() { };
        ~ShaderBuilder() = default;

        void AddInputLayout(const InputLayout& inputLayout);
        void SetInputLayouts(const InputLayouts inputLayouts);

        void SetName(const std::string& name);
        void SetVertexShader(const std::string& vertexShader);
        void SetPixelShader(const std::string& pixelShader);
        void SetGeometryShader(const std::string& geometryShader);

        void SetDepthFunc(DepthFunc depthFunc);
        void SetTopology(Topology topology);

        Ref<Shader> Create();
    private:
        std::string m_Name;

        InputLayouts m_InputLayouts;
        DepthFunc m_DepthFunc = DepthFunc::Less;
        Topology m_Topology = Topology::TriangleList;
        
        std::string m_VertexShader;
        std::string m_PixelShader;
        std::string m_GeometryShader;
    };
}