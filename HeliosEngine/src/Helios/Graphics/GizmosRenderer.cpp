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
			Vector4 color;
			Vector3 position;
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

		Ref<Shader> quad_gizmos_shader;
		Ref<Shader> standard_gizmos_shader;

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
		
		Ref<VertexBuffer> tool_move_vertex_buffer;
		Ref<IndexBuffer> tool_move_index_buffer;
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

		Helios::GizmosData::GizmosVertex tool_move_vertices[] = {
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.21036774620197413f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.22732435670642043f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0352800020149668f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.18920062382698205f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.2397310686657846f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.06985387454973147f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.16424664967969727f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.24733956165584545f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.10302962131043915f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.13600527772234244f, 0.0f, 0.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.24999755163767587f, 0.0f, 0.0f }, -1.0f },

			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.21036774620197413f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.22732435670642043f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0352800020149668f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.18920062382698205f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.2397310686657846f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.06985387454973147f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.16424664967969727f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.24733956165584545f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.10302962131043915f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.13600527772234244f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.24999755163767587f, 0.0f, 1.0f }, -1.0f },

			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.42073549240394825f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.45464871341284085f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0705600040299336f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.3784012476539641f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.4794621373315692f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.13970774909946293f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.32849329935939453f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.4946791233116909f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.2060592426208783f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.2720105554446849f, 0.0f, 1.0f }, -1.0f },
			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { -0.49999510327535174f, 0.0f, 1.0f }, -1.0f },

			{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.5f }, -1.0f },
		};

		uint32_t tool_move_indecies[] = {
			0, 1, 2,
			0, 2, 3,
			0, 4, 5,
			0, 6, 7,
			0, 8, 9,
			0, 9, 10,
			0, 10, 11,
		};

		gizmosData.quad_gizmos_shader = CreateRef<Shader>(Shader("QuadGizmo", {
			{ "Position", Shader::DataType::Float2 },

			{ "World", Shader::DataType::Matrix4x4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "Color", Shader::DataType::Float4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "Data",  Shader::DataType::Float, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance }
			}));

		gizmosData.standard_gizmos_shader = CreateRef<Shader>(Shader("StandardGizmos", {
			{ "Color", Shader::DataType::Float4 },
			{ "Position", Shader::DataType::Float3 },
			{ "Data",  Shader::DataType::Float }
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
		gizmosData.quads.indexBuffer = IndexBuffer::Create(indices, std::size(indices));

		gizmosData.quads.instanceBuffer = VertexBuffer::Create(sizeof(GizmosData::Quad::QuadInstance) * GizmosData::MaxQuadInstances, BufferUsage::Dynamic);
		gizmosData.quads.instanceBuffer->SetStride<GizmosData::Quad::QuadInstance>();

		gizmosData.tool_move_vertex_buffer = VertexBuffer::Create(tool_move_vertices, sizeof(tool_move_vertices), BufferUsage::Static);
		gizmosData.tool_move_vertex_buffer->SetStride<GizmosData::GizmosVertex>();
		gizmosData.tool_move_index_buffer = IndexBuffer::Create(tool_move_indecies, std::size(tool_move_indecies));

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
		HL_PROFILE_BEGIN("Gizmos Renderer - Flush");
		if (gizmosData.quads.quadInstanceIndex > 0)
		{
			gizmosData.quads.instanceBuffer->SetData(gizmosData.quads.quadInstances, sizeof(GizmosData::Quad::QuadInstance) * gizmosData.quads.quadInstanceIndex);

			gizmosData.quad_gizmos_shader->Bind();
			gizmosData.transformBuffer->BindVS(0);
			gizmosData.quads.vertexBuffer->Bind();
			gizmosData.quads.instanceBuffer->Bind(1u);
			gizmosData.quads.indexBuffer->Bind();

			Graphics::instance->m_deviceContext->DrawIndexedInstanced(6u, gizmosData.quads.quadInstanceIndex, 0u, 0u, 0u);
			gizmosData.quads.quadInstanceIndex = 0u;
		}
		HL_PROFILE_END();
	}

	void GizmosRenderer::DrawQuad(SceneCamera camera, Components::Transform& transform, const Vector3& position, const Vector2& size, const Color& color, float data = -1.0f)
	{
		if(gizmosData.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances)
			Flush();

		HL_PROFILE_BEGIN("GizmosRenderer - (DrawQuad) Calculate Matrix");
		gizmosData.quads.quadInstances[gizmosData.quads.quadInstanceIndex] = {
			Matrix4x4::Transpose(
				Matrix4x4::Scale(size) *
				Matrix4x4::Rotation(
					(Quanterion::Conjugate(transform.rotation)) * camera.GetTransform().rotation
				) *
				Matrix4x4::Translation(position) *
				Matrix4x4::Scale(transform.scale) *
				Matrix4x4::Rotation(transform.rotation) *
				Matrix4x4::Translation(transform.position)
			),
			color,
			data
		};
		HL_PROFILE_END();

		gizmosData.quads.quadInstanceIndex++;
	}

	inline void GizmosRenderer::DrawMeshVertices(SceneCamera camera, Components::Transform& transform, std::vector<MeshVertex>& vertices)
	{
		static Matrix4x4 scale = Matrix4x4::Scale({ 0.1f, 0.1f, 1.0f });
		Matrix4x4 vertexMatrix = scale * Matrix4x4::Rotation(Quanterion::Conjugate(transform.rotation) * camera.GetTransform().rotation);
		Matrix4x4 worldMatrix = transform.GetWorldProjectionRow();

		int i = 0;
		for(auto& v : vertices)
		{
			if (gizmosData.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances)
				Flush();

			gizmosData.quads.quadInstances[gizmosData.quads.quadInstanceIndex] = {
				Matrix4x4::Transpose(
					vertexMatrix *
					Matrix4x4::Translation(v.position) *
					worldMatrix
				),
				Color::Yellow, (float)i
			};
			gizmosData.quads.quadInstanceIndex++;
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
