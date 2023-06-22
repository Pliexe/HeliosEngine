#pragma once

#include "Helios/Core/Base.h"
#include "Buffer.h"
#include "Helios/Translation/Vector.h"
#include "Helios/Utils/MeshBuilder.h"

namespace Helios
{
    class HELIOS_API Mesh
    {
    public:
        Mesh() = delete;
        Mesh(const Mesh&) = default;
        Mesh(std::string name, const MeshVertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount);
        Mesh(std::string name, const MeshBuilder& meshBuilder);
		
        static Ref<Mesh> Create(std::string name, const MeshVertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount);
		static Ref<Mesh> Create(std::string name, const MeshBuilder& meshBuilder);
        static void Destroy(std::string name);

        static Ref<Mesh> GeneratePlane();
        static Ref<Mesh> GenerateCube();
        static Ref<Mesh> GetCubeMesh();
        static Ref<Mesh> GetPlaneMesh();
        static Ref<Mesh> GetCylinderMesh();
        static Ref<Mesh> GetConeMesh();
        static Ref<Mesh> GetSphereMesh();
        static MeshBuilder CreateSphereMesh(uint32_t segments = 16.0f);
        static Ref<Mesh> GetDynamicSphereMesh(uint32_t segments);
        static Ref<Mesh> GetDynamicConeMesh(uint32_t segments);
        static Ref<Mesh> GetDynamicCylinderMesh(uint32_t segments);
        static Ref<Mesh> GetTransformMoveMesh();
        static MeshBuilder CreateGizmosArrow(uint32_t segments = 16.0f);

        void Bind();
        inline uint32_t getVertexCount() const { return m_VertexBuffer->getCount<MeshVertex>(); }
		inline uint32_t getIndexCount() const { return m_IndexBuffer->count(); }
		inline Ref<IndexBuffer> getIndexBuffer() const { return m_IndexBuffer; }
		inline Ref<VertexBuffer> getVertexBuffer() const { return m_VertexBuffer; }

        inline std::vector<MeshVertex>& GetVertices() { return m_Vertices; }

    private:

        static std::unordered_map<std::string, Ref<Mesh>> s_Meshes;
        

		std::vector<MeshVertex> m_Vertices;

        std::string m_Name;

        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;

    };
}
