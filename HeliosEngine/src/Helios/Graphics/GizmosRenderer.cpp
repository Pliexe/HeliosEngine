#include "GizmosRenderer.h"
#include "Helios/Resources/Shader.h"
#include "Helios/Utils/MeshBuilder.h"
#include "Helios/Core/Profiler.h"
#include "Helios/Resources/Material.h"
#include "Helios/Resources/Material.h"
#include "Helios/Scene/EditorCamera.h"

namespace Helios
{
	struct GizmosObject
	{
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		void Bind()
		{
			m_VertexBuffer->Bind();
			m_IndexBuffer->Bind();
		}
	};

	struct GizmosVertex
	{
		Vector3 position;
	};

	struct GizmosInstance
	{
		Matrix4x4 transform;
		Color color;
		float id;
	};

	struct LineGizmos
	{
		Vector3 position;
		Color color;
		int32_t mode;
		float thickness;
		float id;
	};

	struct DynamicGizmos
	{
		Vector3 position;
		Color color;
		float id;
	};

	struct GizmosData
	{
		static const uint32_t MaxQuadInstances = 10000;
		static const uint64_t MaxGizmosInstances = 10000;
		static const uint64_t MaxLineGizmos = 10000;
		static const uint64_t MaxDynamicGizmoVertices = 4000 * 3;

		//Ref<VertexBuffer>

		struct DynamicGizmosConfig
		{
			DynamicGizmos* m_vertices;
			DynamicGizmos* m_verticesPtr;

			Ref<VertexBuffer> m_vertex_buffer;
		} m_dynamic_gizmos;

		struct Lines
		{
			LineGizmos* m_Lines;
			LineGizmos* m_index;

			Ref<VertexBuffer> m_vertexBuffer;
		} m_lines;

		struct Gizmos
		{
			static const uint64_t GizmosObjectCount = 2;

			std::array<GizmosObject, GizmosObjectCount> m_GizmosObjects;
			uint32_t m_GizmosInstancesCount[GizmosObjectCount] = { 0 };
			GizmosInstance m_GizmosInstances[GizmosObjectCount][MaxGizmosInstances];

			GizmosObject* ArrowGizmo = nullptr;
			uint32_t* ArrowGizmoInstanceCount = nullptr;

			GizmosObject* SphereGizmo = nullptr;
			uint32_t* SphereGizmoInstanceCount = nullptr;

			Ref<VertexBuffer> m_InstanceBuffer;
		} m_GizmosInstanceData;

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
		Ref<Shader> tool_gizmos_shader;
		Ref<Shader> line_gizmos_shader;
		Ref<Shader> dynamic_gizmos_shader;

		struct QuadTool
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
		Ref<Material> defaultMaterial;
	};

	GizmosData s_Data;

	GizmosRenderer::~GizmosRenderer()
	{
		delete s_Data.m_lines.m_Lines;
		delete s_Data.m_dynamic_gizmos.m_vertices;
	}

	bool GizmosRenderer::Init()
	{
		s_Data.defaultMaterial = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
		s_Data.m_lines.m_Lines = new LineGizmos[GizmosData::MaxLineGizmos];
		s_Data.m_lines.m_index = s_Data.m_lines.m_Lines;
		s_Data.m_lines.m_vertexBuffer = VertexBuffer::Create(sizeof(LineGizmos) * GizmosData::MaxLineGizmos, BufferUsage::Dynamic);
		s_Data.m_lines.m_vertexBuffer->SetStride<LineGizmos>();

		s_Data.m_dynamic_gizmos.m_vertex_buffer = VertexBuffer::Create(sizeof(DynamicGizmos) * GizmosData::MaxDynamicGizmoVertices, BufferUsage::Dynamic);
		s_Data.m_dynamic_gizmos.m_vertex_buffer->SetStride<DynamicGizmos>();
		s_Data.m_dynamic_gizmos.m_vertices = new DynamicGizmos[GizmosData::MaxDynamicGizmoVertices];
		s_Data.m_dynamic_gizmos.m_verticesPtr = s_Data.m_dynamic_gizmos.m_vertices;


#pragma region Initilization Gizmos
		s_Data.m_GizmosInstanceData.ArrowGizmo = s_Data.m_GizmosInstanceData.m_GizmosObjects.data();
		s_Data.m_GizmosInstanceData.ArrowGizmoInstanceCount = s_Data.m_GizmosInstanceData.m_GizmosInstancesCount;

		s_Data.m_GizmosInstanceData.SphereGizmo = s_Data.m_GizmosInstanceData.m_GizmosObjects.data() + 1;
		s_Data.m_GizmosInstanceData.SphereGizmoInstanceCount = s_Data.m_GizmosInstanceData.m_GizmosInstancesCount + 1;
#pragma endregion

#pragma region Initilization Shaders

		s_Data.standard_gizmos_shader = CreateRef<Shader>(Shader("StandardGizmos", {
			{ "Position", Shader::DataType::Float3 },
			{ "Transform", Shader::DataType::Matrix4x4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "Color", Shader::DataType::Float4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "Id",  Shader::DataType::Float, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance }
		}));

		s_Data.quad_gizmos_shader = CreateRef<Shader>(Shader("QuadGizmo", {
			{ "Position", Shader::DataType::Float2 },

			{ "World", Shader::DataType::Matrix4x4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "Color", Shader::DataType::Float4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "Data",  Shader::DataType::Float, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance }
		}));

		s_Data.line_gizmos_shader = CreateRef<Shader>(Shader("Line", {
			{ "Position", Shader::DataType::Float3 },
			{ "Color", Shader::DataType::Float4 },
			{ "Mode", Shader::DataType::Int32 },
			{ "Thickness", Shader::DataType::Float },
			{ "Id", Shader::DataType::Float }
		}, Shader::Topology::LineList));

		s_Data.dynamic_gizmos_shader = CreateRef<Shader>(Shader("DynamicGizmos", {
			{ "Position", Shader::DataType::Float3 },
			{ "Color", Shader::DataType::Float4 },
			{ "Id", Shader::DataType::Float }
		}));

#pragma endregion

#pragma region Create Gizmos Meshes

		s_Data.m_GizmosInstanceData.m_InstanceBuffer = VertexBuffer::Create(sizeof(GizmosInstance) * GizmosData::MaxGizmosInstances, BufferUsage::Dynamic);
		s_Data.m_GizmosInstanceData.m_InstanceBuffer->SetStride<GizmosInstance>();

#pragma region Create Arrow
		
		MeshBuilder arrowBuilder = Mesh::CreateGizmosArrow();
		
		std::vector<GizmosVertex> arrowVertices;
		
		for (auto& vertex : arrowBuilder.GetVertices())
		{
			arrowVertices.emplace_back(GizmosVertex{ vertex.position });
		}

		s_Data.m_GizmosInstanceData.ArrowGizmo->m_VertexBuffer = VertexBuffer::Create(arrowVertices.data(), arrowVertices.size() * sizeof(GizmosVertex));
		s_Data.m_GizmosInstanceData.ArrowGizmo->m_VertexBuffer->SetStride<GizmosVertex>();
		//s_Data.m_GizmosInstanceData.ArrowGizmo->m_IndexBuffer = IndexBuffer::Create(arrowIndicies.data(), arrowIndicies.Size());
		s_Data.m_GizmosInstanceData.ArrowGizmo->m_IndexBuffer = IndexBuffer::Create((uint32_t*)arrowBuilder.GetTriangles().data(), arrowBuilder.GetTriangles().size() * 3);
		
		
#pragma endregion

#pragma region Create Sphere

		MeshBuilder sphereBuilder = Mesh::CreateSphereMesh();

		std::vector<GizmosVertex> sphereVertices;

		for (auto& vertex : sphereBuilder.GetVertices())
		{
			sphereVertices.emplace_back(GizmosVertex{ vertex.position });
		}

		s_Data.m_GizmosInstanceData.SphereGizmo->m_VertexBuffer = VertexBuffer::Create(sphereVertices.data(), sphereVertices.size() * sizeof(GizmosVertex));
		s_Data.m_GizmosInstanceData.SphereGizmo->m_VertexBuffer->SetStride<GizmosVertex>();
		s_Data.m_GizmosInstanceData.SphereGizmo->m_IndexBuffer = IndexBuffer::Create((uint32_t*)sphereBuilder.GetTriangles().data(), sphereBuilder.GetTriangles().size() * 3);

#pragma endregion

#pragma endregion

		const GizmosData::QuadTool::QuadVertex vertices[] = {
			{ { -0.5f,  0.5f } },
			{ {  0.5f,  0.5f } },
			{ {  0.5f, -0.5f } },
			{ { -0.5f, -0.5f } },
		};

		uint32_t indices[] = {
			0, 1, 2,
			0, 2, 3
		};
		
		s_Data.transformBuffer = ConstantBuffer<GizmosData::TransformData>::Create();
		//s_Data.quads.vertexBuffer = VertexBuffer::Create<GizmosData::QuadTool::QuadVertex>({
		//	{ { -1.0f, -1.0f } },
		//	{ {  1.0f, -1.0f } },
		//	{ {  1.0f,  1.0f } },
		//	{ { -1.0f,  1.0f } }
		//}, BufferUsage::Static);
		//s_Data.quads.indexBuffer = IndexBuffer::Create({ 0, 1, 2, 0, 2, 3 });

		s_Data.quads.vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices), BufferUsage::Static);
		s_Data.quads.vertexBuffer->SetStride<GizmosData::QuadTool::QuadVertex>();
		s_Data.quads.indexBuffer = IndexBuffer::Create(indices, std::size(indices));

		s_Data.quads.instanceBuffer = VertexBuffer::Create(sizeof(GizmosData::QuadTool::QuadInstance) * GizmosData::MaxQuadInstances, BufferUsage::Dynamic);
		s_Data.quads.instanceBuffer->SetStride<GizmosData::QuadTool::QuadInstance>();

		return true; 
	}

	void GizmosRenderer::Begin(Matrix4x4 projection)
	{
		s_Data.transformBuffer->SetData({ projection });
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

		s_Data.transformBuffer->BindVS(0);

		HL_PROFILE_BEGIN("Gizmos Renderer - Quads");
		if (s_Data.quads.quadInstanceIndex > 0)
		{
			s_Data.quads.instanceBuffer->SetData(s_Data.quads.quadInstances, sizeof(GizmosData::QuadTool::QuadInstance) * s_Data.quads.quadInstanceIndex);

			s_Data.quad_gizmos_shader->Bind();
			s_Data.quads.vertexBuffer->Bind();
			s_Data.quads.instanceBuffer->Bind(1u);
			s_Data.quads.indexBuffer->Bind();

			Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexedInstanced(6u, s_Data.quads.quadInstanceIndex, 0u, 0u, 0u);
			s_Data.quads.quadInstanceIndex = 0u;
		}
		HL_PROFILE_END();

		HL_PROFILE_BEGIN("Gizmos Renderer - Obj Gizmos");
		uint32_t i = 0;
		for (auto& instances : s_Data.m_GizmosInstanceData.m_GizmosInstancesCount)
		{
			if (instances)
			{
				auto x = *s_Data.m_GizmosInstanceData.m_GizmosInstances[i];

				s_Data.m_GizmosInstanceData.m_InstanceBuffer->SetData(s_Data.m_GizmosInstanceData.m_GizmosInstances[i], sizeof(GizmosInstance) * instances);

				s_Data.standard_gizmos_shader->Bind();
				s_Data.m_GizmosInstanceData.m_GizmosObjects[i].Bind();
				s_Data.m_GizmosInstanceData.m_InstanceBuffer->Bind(1u);
				
				Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexedInstanced(s_Data.m_GizmosInstanceData.m_GizmosObjects[i].m_IndexBuffer->count(), instances, 0u, 0u, 0u);
				instances = 0u;
			}
			i++;
		}
		HL_PROFILE_END();

		HL_PROFILE_BEGIN("Gizmos Renderer - Lines");
		if ((s_Data.m_lines.m_index - s_Data.m_lines.m_Lines) > 0)
		{
			s_Data.m_lines.m_vertexBuffer->SetData(s_Data.m_lines.m_Lines, (s_Data.m_lines.m_index - s_Data.m_lines.m_Lines) * sizeof(LineGizmos));
			s_Data.line_gizmos_shader->Bind();
			s_Data.m_lines.m_vertexBuffer->Bind();
			s_Data.transformBuffer->BindGS(0);

			Direct3D11Context::GetCurrentContext()->GetContext()->Draw((s_Data.m_lines.m_index - s_Data.m_lines.m_Lines), 0u);
			s_Data.m_lines.m_index = s_Data.m_lines.m_Lines;
			s_Data.line_gizmos_shader->Unbind();
			//s_Data.m_lines.m_vertexBuffer->Unbind();
		}
		HL_PROFILE_END();

		HL_PROFILE_BEGIN("Gizmos Renderer - Dynamic Lines");
		if ((s_Data.m_dynamic_gizmos.m_verticesPtr - s_Data.m_dynamic_gizmos.m_vertices) > 0)
		{
			s_Data.m_dynamic_gizmos.m_vertex_buffer->SetData(s_Data.m_dynamic_gizmos.m_vertices, (s_Data.m_dynamic_gizmos.m_verticesPtr - s_Data.m_dynamic_gizmos.m_vertices) * sizeof(DynamicGizmos));
			s_Data.dynamic_gizmos_shader->Bind();
			s_Data.m_dynamic_gizmos.m_vertex_buffer->Bind();

			Direct3D11Context::GetCurrentContext()->GetContext()->Draw((s_Data.m_dynamic_gizmos.m_verticesPtr - s_Data.m_dynamic_gizmos.m_vertices), 0u);
			s_Data.m_dynamic_gizmos.m_verticesPtr = s_Data.m_dynamic_gizmos.m_vertices;
			s_Data.dynamic_gizmos_shader->Unbind();
		}
		HL_PROFILE_END();

		HL_PROFILE_END();
	}

	void GizmosRenderer::DrawTriangle(Vector3 a, Vector3 b, Vector3 c, Color color, float id)
	{
		if (s_Data.m_dynamic_gizmos.m_verticesPtr + 3 >= s_Data.m_dynamic_gizmos.m_vertices + GizmosData::MaxDynamicGizmoVertices)
			Flush();

		s_Data.m_dynamic_gizmos.m_verticesPtr->position = a;
		s_Data.m_dynamic_gizmos.m_verticesPtr->color = color;
		s_Data.m_dynamic_gizmos.m_verticesPtr->id = id;
		s_Data.m_dynamic_gizmos.m_verticesPtr++;

		s_Data.m_dynamic_gizmos.m_verticesPtr->position = b;
		s_Data.m_dynamic_gizmos.m_verticesPtr->color = color;
		s_Data.m_dynamic_gizmos.m_verticesPtr->id = id;
		s_Data.m_dynamic_gizmos.m_verticesPtr++;

		s_Data.m_dynamic_gizmos.m_verticesPtr->position = c;
		s_Data.m_dynamic_gizmos.m_verticesPtr->color = color;
		s_Data.m_dynamic_gizmos.m_verticesPtr->id = id;
		s_Data.m_dynamic_gizmos.m_verticesPtr++;
	}

#define MAX_CIRCLE_SEGMENTS 64

	void GizmosRenderer::DrawAngle(Vector3 a_normal, Vector3 b_normal, float radius, Color color, Matrix4x4 model_matrix)
	{
		if (s_Data.m_dynamic_gizmos.m_verticesPtr + MAX_CIRCLE_SEGMENTS >= s_Data.m_dynamic_gizmos.m_vertices + GizmosData::MaxDynamicGizmoVertices)
			Flush();

		// Circle with angle between a_normal and b_normal, The amount of segments is based on the radius
		// Double sided circle
		// Apply model matrix to the circle
	}

	void GizmosRenderer::DrawAngle(float angle, float radius, Color color, Matrix4x4 model_matrix)
	{
		
	}

	void GizmosRenderer::DrawAngle(Vector3 root, Vector3 a_normal, Vector3 b_normal, float radius, Color color, Matrix4x4 model_matrix)
	{
		if (s_Data.m_dynamic_gizmos.m_verticesPtr + MAX_CIRCLE_SEGMENTS >= s_Data.m_dynamic_gizmos.m_vertices + GizmosData::MaxDynamicGizmoVertices)
			Flush();

		// Draw a 2 sided circle with angle between a_normal and b_normal, The amount of segments is based on the angle and model matrix is applied to the circle and draw lines at edges of the circle using DrawLine

		float angle = acosf(Vector3::Dot(a_normal, b_normal));
		float segments = angle * radius;
		if (segments > MAX_CIRCLE_SEGMENTS)
			segments = MAX_CIRCLE_SEGMENTS;

		float angle_step = angle / segments;
		float current_angle = 0.0f;

		Vector3 current_normal = a_normal;
		Vector3 next_normal = a_normal;

		DrawLine(root, root + a_normal * radius, 0.5f, color, -1, LineMode::Rounded_Solid);

		for (int i = 0; i < segments; i++)
		{
			current_normal = Vector3::Rotate(current_normal, b_normal, angle_step);
			next_normal = Vector3::Rotate(next_normal, b_normal, angle_step);

			s_Data.m_dynamic_gizmos.m_verticesPtr->position = root + current_normal * radius;
			s_Data.m_dynamic_gizmos.m_verticesPtr->color = color;
			s_Data.m_dynamic_gizmos.m_verticesPtr->id = 0.0f;
			s_Data.m_dynamic_gizmos.m_verticesPtr++;

			s_Data.m_dynamic_gizmos.m_verticesPtr->position = root + next_normal * radius;
			s_Data.m_dynamic_gizmos.m_verticesPtr->color = color;
			s_Data.m_dynamic_gizmos.m_verticesPtr->id = 0.0f;
			s_Data.m_dynamic_gizmos.m_verticesPtr++;

			s_Data.m_dynamic_gizmos.m_verticesPtr->position = root + current_normal * radius;
			s_Data.m_dynamic_gizmos.m_verticesPtr->color = color;
			s_Data.m_dynamic_gizmos.m_verticesPtr->id = 0.0f;
			s_Data.m_dynamic_gizmos.m_verticesPtr++;

			s_Data.m_dynamic_gizmos.m_verticesPtr->position = root + next_normal * radius;
			s_Data.m_dynamic_gizmos.m_verticesPtr->color = color;
			s_Data.m_dynamic_gizmos.m_verticesPtr->id = 0.0f;
			s_Data.m_dynamic_gizmos.m_verticesPtr++;

			s_Data.m_dynamic_gizmos.m_verticesPtr->position = root + next_normal * radius;
			s_Data.m_dynamic_gizmos.m_verticesPtr->color = color;
			s_Data.m_dynamic_gizmos.m_verticesPtr->id = 0.0f;
			s_Data.m_dynamic_gizmos.m_verticesPtr++;

			s_Data.m_dynamic_gizmos.m_verticesPtr->position = root + current_normal * radius;
			s_Data.m_dynamic_gizmos.m_verticesPtr->color = color;
			s_Data.m_dynamic_gizmos.m_verticesPtr->id = 0.0f;
			s_Data.m_dynamic_gizmos.m_verticesPtr++;

			DrawLine(root + current_normal * radius, root + next_normal * radius, 0.5f, color, -1, LineMode::Rounded_Solid);
		}

		DrawLine(root, root + b_normal * radius, 0.5f, color, -1, LineMode::Rounded_Solid);
	}

	void GizmosRenderer::DrawLine(Vector3 a, Vector3 b, float width, Color color, int64_t id, LineMode mode)
	{
		if ((s_Data.m_lines.m_index - s_Data.m_lines.m_Lines) >= GizmosData::MaxLineGizmos - 1)
			Flush();

		*s_Data.m_lines.m_index = LineGizmos{
			a, color, (int32_t)mode, width, (float)id
		};
		s_Data.m_lines.m_index++;
		*s_Data.m_lines.m_index = LineGizmos{
			b, color, (int32_t)mode, width, (float)id
		};
		s_Data.m_lines.m_index++;
	}

	void GizmosRenderer::DrawQuad(SceneCamera camera, TransformComponent& transform, const Vector3& position, const Vector2& size, const Color& color, float data = -1.0f)
	{
		if(s_Data.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances)
			Flush();

		HL_PROFILE_BEGIN("GizmosRenderer - (DrawQuad) Calculate Matrix");
		s_Data.quads.quadInstances[s_Data.quads.quadInstanceIndex] = {
			Matrix4x4::Transpose(
				Matrix4x4::Scale(size) *
				Matrix4x4::RotationRow(
					(Quaternion::Conjugate(transform.Rotation)) * camera.GetTransform().Rotation
				) *
				Matrix4x4::TranslationRow(position) *
				Matrix4x4::Scale(transform.Scale) *
				Matrix4x4::RotationRow(transform.Rotation) *
				Matrix4x4::TranslationRow(transform.Position)
			),
			color,
			data
		};
		HL_PROFILE_END();

		s_Data.quads.quadInstanceIndex++;
	}

	inline void GizmosRenderer::DrawMeshVertices(EditorCamera camera, TransformComponent& transform, std::vector<MeshVertex>& vertices)
	{
		static Matrix4x4 scale = Matrix4x4::Scale({ 0.1f, 0.1f, 1.0f });
		Matrix4x4 vertexMatrix = scale * Matrix4x4::RotationRow(Quaternion::Conjugate(transform.Rotation) * camera.GetRotation());
		Matrix4x4 worldMatrix = transform.GetRowModelMatrix();

		int i = 0;
		for(auto& v : vertices)
		{
			if (s_Data.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances)
				Flush();

			s_Data.quads.quadInstances[s_Data.quads.quadInstanceIndex] = {
				Matrix4x4::Transpose(
					vertexMatrix *
					Matrix4x4::TranslationRow(v.position) *
					worldMatrix
				),
				Color::Yellow, (float)i
			};
			s_Data.quads.quadInstanceIndex++;
			i++;
		}
	}

	void GizmosRenderer::DrawTool(Matrix4x4 transform, ToolType type, Tool operation)
	{
		switch (type)
		{
		case ToolType::Move:
		{
			

			static GizmosData::QuadTool::QuadInstance xx[] = {
				{ { }, Color::Red, (float)Tool::MoveZY },
				{ { }, Color::Red, (float)Tool::MoveZY },
				{ { }, Color::Green, (float)Tool::MoveXY },
				{ { }, Color::Green, (float)Tool::MoveXY },
				{ { }, Color::Blue, (float)Tool::MoveXZ },
				{ { }, Color::Blue, (float)Tool::MoveXZ },
			};

			if (*s_Data.m_GizmosInstanceData.ArrowGizmoInstanceCount >= GizmosData::MaxGizmosInstances - 3)
				Flush();

			if (s_Data.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances - std::size(xx) - 5)
				Flush();

			GizmosInstance x[4] = {
				{ { }, operation == Tool::MoveY ? Color::Yellow : Color::Blue, (float)Tool::MoveY },
				{ { }, operation == Tool::MoveX ? Color::Yellow : Color::Red, (float)Tool::MoveX },
				{ { }, operation == Tool::MoveZ ? Color::Yellow : Color::Green, (float)Tool::MoveZ },
			};

			x[0].transform = transform;
			x[1].transform = x[0].transform * Matrix4x4::RotationZRow(-(PI * 0.5f));
			x[2].transform = x[1].transform * Matrix4x4::RotationXRow(-(PI * 0.5f));

			GizmosInstance* ptr = &s_Data.m_GizmosInstanceData.m_GizmosInstances[0][*s_Data.m_GizmosInstanceData.ArrowGizmoInstanceCount];

			memcpy(ptr, x, sizeof(x));
			(*s_Data.m_GizmosInstanceData.ArrowGizmoInstanceCount) += std::size(x);


			static const float scale = 0.2f; 
			static const auto xx1 = Matrix4x4::TranslationColumn(0.0f, 0.5f, 0.5f) * Matrix4x4::Scale(scale) * Matrix4x4::RotationYRow(-(PI * 0.5f));
			static const auto xy1 = xx1 * Matrix4x4::RotationXRow(PI);

			static const auto xx2 = Matrix4x4::TranslationColumn(0.5f, 0.5f, 0.0f) * Matrix4x4::Scale(scale);
			static const auto xy2 = xx2 * Matrix4x4::RotationXRow(PI);

			static const auto xx3 = Matrix4x4::TranslationColumn(0.5f, 0.0f, 0.5f) * Matrix4x4::Scale(scale) * Matrix4x4::RotationXRow(-(PI * 0.5f));
			static const auto xy3 = xx3 * Matrix4x4::RotationXRow(PI);

			xx[0].position = transform * xx1;
			xx[1].position = transform * xy1;

			xx[2].position = transform * xx2;
			xx[3].position = transform * xy2;

			xx[4].position = transform * xx3;
			xx[5].position = transform * xy3;

			memcpy(&s_Data.quads.quadInstances[s_Data.quads.quadInstanceIndex], xx, sizeof(xx));
			s_Data.quads.quadInstanceIndex += std::size(xx);

			s_Data.m_GizmosInstanceData.m_GizmosInstances[1][*s_Data.m_GizmosInstanceData.SphereGizmoInstanceCount] = { transform * Matrix4x4::Scale(0.1f), operation == Tool::MoveXYZ ? Color::Yellow : Color::White, (float)Tool::MoveXYZ};
			(*s_Data.m_GizmosInstanceData.SphereGizmoInstanceCount)++;

			break;
		}
		default:
			break;
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
