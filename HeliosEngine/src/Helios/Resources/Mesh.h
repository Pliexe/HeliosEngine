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
        Mesh(std::string name, const MeshVertexData* vertices, uint32_t vertexCount, const unsigned short* indices, uint32_t indexCount);
		
        static Ref<Mesh> Create(std::string name, const MeshVertexData* vertices, uint32_t vertexCount, const unsigned short* indices, uint32_t indexCount);
        static void Destroy(std::string name);

        static Ref<Mesh> GetCubeMesh();
        static Ref<Mesh> GetTransformMoveMesh();

        void Bind();
		inline uint32_t GetIndexCount() const { return m_IndexBuffer->GetCount(); }

    private:

        static std::unordered_map<std::string, Ref<Mesh>> s_Meshes;

        std::string m_Name;

        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;

    };
}