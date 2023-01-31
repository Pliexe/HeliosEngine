#pragma once

#include "Helios/Core/Base.h"
#include "Buffer.h"
#include "Helios/Translation/Vector.h"

namespace Helios
{
    struct MeshVertexData
    {
        Vector3 position;
        Vector2 texCoord;
    };

    class HELIOS_API Mesh
    {
    public:
        Mesh() = delete;
        Mesh(const Mesh&) = default;
        Mesh(std::string name, const MeshVertexData* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount);
		
        static Ref<Mesh> Create(std::string name, const MeshVertexData* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount);
        static void Destroy(std::string name);

        static Ref<Mesh> GetCubeMesh();
		static Ref<Mesh> GetCylinderMesh();
        static Ref<Mesh> GetDynamicCylinderMesh(uint32_t sides);
        static Ref<Mesh> GetTransformMoveMesh();
		

        void Bind();
        inline uint32_t getVertexCount() const { return m_VertexBuffer->getCount<MeshVertexData>(); }
		inline uint32_t getIndexCount() const { return m_IndexBuffer->GetCount(); }
		inline Ref<IndexBuffer> getIndexBuffer() const { return m_IndexBuffer; }
		inline Ref<VertexBuffer> getVertexBuffer() const { return m_VertexBuffer; }
		

    private:

        static std::unordered_map<std::string, Ref<Mesh>> s_Meshes;

		uint32_t m_VertexCount;
		uint32_t m_IndexCount;

        std::string m_Name;

        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;

    };
}