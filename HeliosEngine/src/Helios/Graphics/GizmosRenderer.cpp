#include "GizmosRenderer.h"
#include "Helios/Resources/Shader.h"
#include "Helios/Utils/MeshBuilder.h"
#include "Helios/Core/Profiler.h"

namespace Helios
{
	struct GizmosData
	{
		static const uint32_t MaxQuadInstances = 10000;

		struct GizmosVertex
		{
			Vector3 position;
			Vector4 color;
			float id;
		};

		struct VertexLocationDotVertex
		{
			Vector3 position;
			Vector4 color;
			float index;
		};

		struct TransformData
		{
			Matrix4x4 viewProjection;
		};

		Ref<Shader> gizmosShader;

		struct Quad
		{
			struct QuadVertex
			{
				Vector2 position;
			};

			struct QuadInstance
			{
				Matrix4x4 position;
				Color color;
				float data;
			};

			Ref<VertexBuffer> vertexBuffer;
			Ref<VertexBuffer> instanceBuffer;
			Ref<IndexBuffer> indexBuffer;

			QuadInstance quadInstances[MaxQuadInstances];
			uint32_t quadInstanceIndex = 0;
		} quads;

		Ref<ConstantBuffer<TransformData>> transformBuffer;

	};

	GizmosData gizmosData;

	bool GizmosRenderer::Init()
	{
		const GizmosData::Quad::QuadVertex vertices[] = {
			{ { -0.5f,  0.5f } },
			{ {  0.5f,  0.5f } },
			{ {  0.5f, -0.5f } },
			{ { -0.5f, -0.5f } },
		};

		uint32_t indices[] = {
			0, 1, 2,
			0, 2, 3
		};

		gizmosData.gizmosShader = CreateRef<Shader>(Shader("QuadGizmo", {
			{ "Position", Shader::DataType::Float2 },

			{ "World", Shader::DataType::Matrix4x4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "Color", Shader::DataType::Float4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "Data",  Shader::DataType::Float, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance }
		}));
		gizmosData.transformBuffer = ConstantBuffer<GizmosData::TransformData>::Create();
		//gizmosData.quads.vertexBuffer = VertexBuffer::Create<GizmosData::Quad::QuadVertex>({
		//	{ { -1.0f, -1.0f } },
		//	{ {  1.0f, -1.0f } },
		//	{ {  1.0f,  1.0f } },
		//	{ { -1.0f,  1.0f } }
		//}, BufferUsage::Static);
		//gizmosData.quads.indexBuffer = IndexBuffer::Create({ 0, 1, 2, 0, 2, 3 });

		gizmosData.quads.vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices), BufferUsage::Static);
		gizmosData.quads.vertexBuffer->SetStride<GizmosData::Quad::QuadVertex>();
		gizmosData.quads.indexBuffer = IndexBuffer::Create(indices, sizeof(indices));

		gizmosData.quads.instanceBuffer = VertexBuffer::Create(sizeof(GizmosData::Quad::QuadInstance) * GizmosData::MaxQuadInstances, BufferUsage::Dynamic);
		gizmosData.quads.instanceBuffer->SetStride<GizmosData::Quad::QuadInstance>();

		return true; 
	}

	void GizmosRenderer::Begin(Matrix4x4 projection)
	{
		gizmosData.transformBuffer->SetData({ projection });
	}

	void GizmosRenderer::End()
	{
		Flush();
	}

	void GizmosRenderer::RenderVertices()
	{

	}

	void GizmosRenderer::Flush()
	{
		if (gizmosData.quads.quadInstanceIndex > 0)
		{
			gizmosData.quads.instanceBuffer->SetData(gizmosData.quads.quadInstances, sizeof(GizmosData::Quad::QuadInstance) * gizmosData.quads.quadInstanceIndex);

			gizmosData.gizmosShader->Bind();
			gizmosData.transformBuffer->BindVS(0);
			gizmosData.quads.vertexBuffer->Bind();
			gizmosData.quads.instanceBuffer->Bind(1u);
			gizmosData.quads.indexBuffer->Bind();

			Graphics::instance->m_deviceContext->DrawIndexedInstanced(6u, gizmosData.quads.quadInstanceIndex, 0u, 0u, 0u);
			gizmosData.quads.quadInstanceIndex = 0u;
		}
	}

	void GizmosRenderer::DrawQuad(SceneCamera camera, Components::Transform& transform, const Vector3& position, const Vector2& size, const Color& color, float data = -1.0f)
	{
		if(gizmosData.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances)
			Flush();

		gizmosData.quads.quadInstances[gizmosData.quads.quadInstanceIndex] = {
			Matrix4x4::Transpose(
				Matrix4x4::Scale(size) *
				// Rotate transform.position + position point in world space to face camera.GetTransform().position
				Matrix4x4::Rotation(
					/*Quanterion::FromEulerRads((camera.GetTransform().position - (transform.position + position)).normalize()) * transform.rotation*/
					(Quanterion::Conjugate(transform.rotation)) * camera.GetTransform().rotation
				) *
				//Matrix4x4::Rotation(Quanterion::FromEulerRads(((transform.position + position) - camera.GetTransform().position).normalize()) * Quanterion::Conjugate(transform.rotation) ) *
				Matrix4x4::Translation(position) *
				Matrix4x4::Scale(transform.scale) *
				Matrix4x4::Rotation(transform.rotation) *
				/*Matrix4x4::Rotation(
					Quanterion::FromEulerRads((camera.GetTransform().position - transform.position).normalize()) * transform.rotation
				) **/
				Matrix4x4::Translation(transform.position)
			),
			color,
			data
		};

		gizmosData.quads.quadInstanceIndex++;
	}

	inline void GizmosRenderer::DrawMeshVertices(SceneCamera camera, Components::Transform& transform, std::vector<MeshVertex>& vertices)
	{
		int i = 0;
		for(auto& v : vertices)
		{
			DrawQuad(camera, transform, v.position, { 0.1f, 0.1f }, Color::Yellow, i);
			i++;
		}
	}

	inline void GizmosRenderer::SetDisplayedVertices(Ref<std::vector<MeshVertex>> vertices)
	{
		//m_DisplayedVertices = quads;
	}

	inline void GizmosRenderer::ClearDisplayedVertices()
	{
		//m_DisplayedVertices = nullptr;
	}
}
