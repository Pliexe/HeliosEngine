#pragma once

#include "Helios/Core/Base.h"
#include "Buffer.h"
#include "Helios/Math/Vector.h"
#include "Helios/Utils/MeshBuilder.h"
#include "Helios/Core/UUID.h"

namespace Helios
{
    class HELIOS_API Mesh
    {
    public:
        Mesh() = delete;
        Mesh(const Mesh&) = default;
        Mesh(std::string name, const UUID& uuid, const MeshVertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount);
        Mesh(std::string name, const UUID& uuid, const MeshBuilder& meshBuilder);
		
        static Ref<Mesh> Create(std::string name, const UUID& uuid, const MeshVertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount);
        static Ref<Mesh> Create(std::string name, const UUID& uuid, const MeshBuilder& meshBuilder);
      
        static Ref<Mesh> GeneratePlane();
        static Ref<Mesh> GenerateCube();
        static Ref<Mesh> GetCubeMesh();
        static Ref<Mesh> GetPlaneMesh();
        static Ref<Mesh> GetCylinderMesh();
        static Ref<Mesh> GetConeMesh();
        static Ref<Mesh> GetSphereMesh();
        static Ref<Mesh> GetTorusMesh(uint32_t segments_ring, uint32_t segments_cylinder, float R, float r);
        static Ref<Mesh> GetDynamicSphereMesh(uint32_t segments);
        static Ref<Mesh> GetDynamicConeMesh(uint32_t segments);
        static Ref<Mesh> GetDynamicCylinderMesh(uint32_t segments);
        static Ref<Mesh> GetTransformMoveMesh();
        static MeshBuilder CreateSphereMesh(uint32_t segments = 16.0f);
        static MeshBuilder CreateTorus(uint32_t segments_ring, uint32_t segments_cylinder, float R, float r);
        static MeshBuilder CreateGizmosArrow(uint32_t segments = 16.0f);

        static const uint32_t GetMaxInstanceCount() { return 100000; }
        static const Ref<UnsafeVertexBuffer>& GetInstanceVertexBuffer()
        { 
            if (!s_InstanceVertexBuffer)
                s_InstanceVertexBuffer = UnsafeVertexBuffer::Create(GetMaxInstanceCount(), BufferUsage::Dynamic);
            return s_InstanceVertexBuffer;
        }

        static const SafeInputLayouts<2> GetInputLayout()
        {
            static SafeInputLayouts<2> layouts({
                InputLayout {
                    { "Position", ShaderDataType::Float32_3 },
                    { "TexCoord", ShaderDataType::Float32_2 },
                    { "Normal", ShaderDataType::Float32_3 }
                },
                InputLayout {
                    { "WorldViewProj", ShaderDataType::MatrixFloat4x4 },
                    { "WorldProj", ShaderDataType::MatrixFloat4x4 },
                    { "Color", ShaderDataType::Float32_4 },
                    { "EntityId", ShaderDataType::Int32 },
                    { "SceneIndex", ShaderDataType::Int32 }
                }
            });
            return layouts;
        }

        void Bind();
        inline uint32_t getVertexCount() const { return /*m_VertexBuffer->getCount<MeshVertex>();*/ -1; }
        inline uint32_t getIndexCount() const { return m_VertexArray->GetIndexBuffer()->GetCount(); /*m_IndexBuffer->GetCount();*/ }
		inline Ref<IndexBuffer> getIndexBuffer() const { return m_IndexBuffer; }
		inline Ref<UnsafeVertexBuffer> getVertexBuffer() const { return (m_VertexArray->GetVertexBuffers()[0]); }

        inline std::vector<MeshVertex>& GetVertices() { return m_Vertices; }

		inline std::string& GetName() { return m_Name; }
		inline const Helios::UUID& GetID() { return m_ID; }

    private:

		inline static Ref<UnsafeVertexBuffer> s_InstanceVertexBuffer;

		std::vector<MeshVertex> m_Vertices;

        std::string m_Name;
		const Helios::UUID m_ID;

        //Ref<VertexBuffer<MeshVertex>> m_VertexBuffer;
		Ref<VertexArray> m_VertexArray;
        Ref<IndexBuffer> m_IndexBuffer;

    };
}
