#include "GizmosRenderer.h"
#include "Helios/Resources/Shader.h"
#include "Helios/Utils/MeshBuilder.h"
#include "Helios/Core/Profiler.h"
#include "Helios/Resources/Material.h"
#include "Helios/Resources/Material.h"
#include "Helios/Scene/EditorCamera.h"
#include "Helios/Scene/Entity.h"

namespace Helios
{
	struct GizmosObject
	{
		Ref<UnsafeVertexBuffer> m_VertexBuffer;
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
		int32_t id;
	};

	struct LineGizmos
	{
		Vector3 position;
		Color color;
		int32_t mode;
		float thickness;
		int32_t id;
	};

	struct DynamicGizmos
	{
		Vector3 position;
		Color color;
		int32_t id;
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

			Ref<UnsafeVertexBuffer> m_vertex_buffer;
		} m_dynamic_gizmos;

		struct Lines
		{
			LineGizmos* m_Lines;
			LineGizmos* m_index;

			Ref<UnsafeVertexBuffer> m_vertexBuffer;
		} m_lines;

		struct Gizmos
		{
			static const uint64_t GizmosObjectCount = 3;

			std::array<GizmosObject, GizmosObjectCount> m_GizmosObjects;
			uint32_t m_GizmosInstancesCount[GizmosObjectCount] = { 0 };
			GizmosInstance m_GizmosInstances[GizmosObjectCount][MaxGizmosInstances];

			GizmosObject* ArrowGizmo = nullptr;
			uint32_t* ArrowGizmoInstanceCount = nullptr;

			GizmosObject* SphereGizmo = nullptr;
			uint32_t* SphereGizmoInstanceCount = nullptr;

			GizmosObject* TorusGizmo = nullptr;
			uint32_t* TorusGizmoInstanceCount = nullptr;

			Ref<UnsafeVertexBuffer> m_InstanceBuffer;
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

		InputLayouts quad_gizmos_input_layouts;
		InputLayouts standard_gizmos_input_layouts;
		InputLayouts tool_gizmos_input_layouts;
		InputLayouts line_gizmos_input_layouts;
		InputLayouts dynamic_gizmos_input_layouts;

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
				int data;
			};

			Ref<VertexArray> vertexArray;

			QuadInstance quadInstances[MaxQuadInstances];
			uint32_t quadInstanceIndex = 0;
		} quads;

		Ref<UniformBuffer<TransformData>> transformBuffer;
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

#pragma region Initilization Input Layouts

		s_Data.standard_gizmos_input_layouts = InputLayouts{
			InputLayout { { "Position", ShaderDataType::Float32_3 } },
			InputLayout {
				{ "Transform", ShaderDataType::MatrixFloat4x4 },
				{ "Color", ShaderDataType::Float32_4 },
				{ "Id", ShaderDataType::Int32 }
			}
		};

		s_Data.quad_gizmos_input_layouts = InputLayouts{
			InputLayout { { "Position", ShaderDataType::Float32_2 } },
			InputLayout {
				{ "World", ShaderDataType::MatrixFloat4x4 },
				{ "Color", ShaderDataType::Float32_4 },
				{ "Data", ShaderDataType::Int32 }
			}
		};

		s_Data.line_gizmos_input_layouts = InputLayouts{
			InputLayout {
				{ "Position", ShaderDataType::Float32_3 },
				{ "Color", ShaderDataType::Float32_4 },
				{ "Mode", ShaderDataType::Int32 },
				{ "Thickness", ShaderDataType::Float32 },
				{ "Id", ShaderDataType::Int32 }
			}
		};

		s_Data.dynamic_gizmos_input_layouts = InputLayouts{
			InputLayout {
				{ "Position", ShaderDataType::Float32_3 },
				{ "Color", ShaderDataType::Float32_4 },
				{ "Id", ShaderDataType::Int32 }
			}
		};

#pragma endregion

		s_Data.defaultMaterial = Material::Create(Material::Filter::MinMagMipPoint, Material::Type::Warp);
		s_Data.m_lines.m_Lines = new LineGizmos[GizmosData::MaxLineGizmos];
		s_Data.m_lines.m_index = s_Data.m_lines.m_Lines;
		s_Data.m_lines.m_vertexBuffer = UnsafeVertexBuffer::Create(sizeof(LineGizmos) * GizmosData::MaxLineGizmos, BufferUsage::Dynamic);
		s_Data.m_lines.m_vertexBuffer->SetInputLayout(s_Data.line_gizmos_input_layouts[0]);

		s_Data.m_dynamic_gizmos.m_vertex_buffer = UnsafeVertexBuffer::Create(sizeof(DynamicGizmos) * GizmosData::MaxDynamicGizmoVertices, BufferUsage::Dynamic);
		s_Data.m_dynamic_gizmos.m_vertex_buffer->SetInputLayout(s_Data.dynamic_gizmos_input_layouts[0]);
		s_Data.m_dynamic_gizmos.m_vertices = new DynamicGizmos[GizmosData::MaxDynamicGizmoVertices];
		s_Data.m_dynamic_gizmos.m_verticesPtr = s_Data.m_dynamic_gizmos.m_vertices;


#pragma region Initilization Gizmos
		s_Data.m_GizmosInstanceData.ArrowGizmo = s_Data.m_GizmosInstanceData.m_GizmosObjects.data();
		s_Data.m_GizmosInstanceData.ArrowGizmoInstanceCount = s_Data.m_GizmosInstanceData.m_GizmosInstancesCount;

		s_Data.m_GizmosInstanceData.SphereGizmo = s_Data.m_GizmosInstanceData.m_GizmosObjects.data() + 1;
		s_Data.m_GizmosInstanceData.SphereGizmoInstanceCount = s_Data.m_GizmosInstanceData.m_GizmosInstancesCount + 1;

		s_Data.m_GizmosInstanceData.TorusGizmo = s_Data.m_GizmosInstanceData.m_GizmosObjects.data() + 2;
		s_Data.m_GizmosInstanceData.TorusGizmoInstanceCount = s_Data.m_GizmosInstanceData.m_GizmosInstancesCount + 2;

#pragma endregion

#pragma region Initilization Shaders

		s_Data.standard_gizmos_shader = Shader::Create("StandardGizmos", "CompiledShaders/StandardGizmosVertexShader", "CompiledShaders/StandardGizmosPixelShader", s_Data.standard_gizmos_input_layouts, DepthFunc::Always, Topology::TriangleList);

		s_Data.quad_gizmos_shader = Shader::Create("QuadGizmo", "CompiledShaders/QuadGizmoVertexShader", "CompiledShaders/QuadGizmoPixelShader", s_Data.quad_gizmos_input_layouts, DepthFunc::Always, Topology::TriangleList);

		s_Data.line_gizmos_shader = Shader::Create("Line", "CompiledShaders/LineVertexShader", "CompiledShaders/LinePixelShader", "CompiledShaders/LineGeometryShader", s_Data.line_gizmos_input_layouts, DepthFunc::Always, Topology::LineList);

		s_Data.dynamic_gizmos_shader = Shader::Create("DynamicGizmos", "CompiledShaders/DynamicGizmosVertexShader", "CompiledShaders/DynamicGizmosPixelShader", s_Data.dynamic_gizmos_input_layouts, DepthFunc::Always, Topology::TriangleList);

#pragma endregion

#pragma region Create Gizmos Meshes

		s_Data.m_GizmosInstanceData.m_InstanceBuffer = UnsafeVertexBuffer::Create(sizeof(GizmosInstance) * GizmosData::MaxGizmosInstances, BufferUsage::Dynamic);
		s_Data.m_GizmosInstanceData.m_InstanceBuffer->SetInputLayout(s_Data.standard_gizmos_input_layouts[1]);

#pragma region Create Arrow
		
		MeshBuilder arrowBuilder = Mesh::CreateGizmosArrow();
		
		std::vector<GizmosVertex> arrowVertices;
		
		for (auto& vertex : arrowBuilder.GetVertices())
		{
			arrowVertices.emplace_back(GizmosVertex{ vertex.position });
		}

		s_Data.m_GizmosInstanceData.ArrowGizmo->m_VertexBuffer = UnsafeVertexBuffer::Create(arrowVertices.data(), arrowVertices.size() * sizeof(GizmosVertex));
		s_Data.m_GizmosInstanceData.ArrowGizmo->m_VertexBuffer->SetInputLayout(s_Data.standard_gizmos_input_layouts[0]);
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

		s_Data.m_GizmosInstanceData.SphereGizmo->m_VertexBuffer = UnsafeVertexBuffer::Create(sphereVertices.data(), sphereVertices.size() * sizeof(GizmosVertex));
		s_Data.m_GizmosInstanceData.SphereGizmo->m_VertexBuffer->SetInputLayout(s_Data.standard_gizmos_input_layouts[0]);
		s_Data.m_GizmosInstanceData.SphereGizmo->m_IndexBuffer = IndexBuffer::Create((uint32_t*)sphereBuilder.GetTriangles().data(), sphereBuilder.GetTriangles().size() * 3);

#pragma endregion

		MeshBuilder torusBuilder = Mesh::CreateTorus(32, 32, 1.0f, 0.01f);

		std::vector<GizmosVertex> torusVertices;

		for (auto& vertex : torusBuilder.GetVertices())
		{
			torusVertices.emplace_back(GizmosVertex{ vertex.position });
		}

		s_Data.m_GizmosInstanceData.TorusGizmo->m_VertexBuffer = UnsafeVertexBuffer::Create(torusVertices.data(), torusVertices.size() * sizeof(GizmosVertex));
		s_Data.m_GizmosInstanceData.TorusGizmo->m_VertexBuffer->SetInputLayout(s_Data.standard_gizmos_input_layouts[0]);
		s_Data.m_GizmosInstanceData.TorusGizmo->m_IndexBuffer = IndexBuffer::Create((uint32_t*)torusBuilder.GetTriangles().data(), torusBuilder.GetTriangles().size() * 3);

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
		
		s_Data.transformBuffer = UniformBuffer<GizmosData::TransformData>::Create(0u);
		//s_Data.quads.vertexBuffer = VertexBuffer::Create<GizmosData::QuadTool::QuadVertex>({
		//	{ { -1.0f, -1.0f } },
		//	{ {  1.0f, -1.0f } },
		//	{ {  1.0f,  1.0f } },
		//	{ { -1.0f,  1.0f } }
		//}, BufferUsage::Static);
		//s_Data.quads.indexBuffer = IndexBuffer::Create({ 0, 1, 2, 0, 2, 3 });

		s_Data.quads.vertexArray = VertexArray::Create(s_Data.quad_gizmos_input_layouts, {
			{ vertices, sizeof(vertices) },
			{ nullptr, sizeof(GizmosData::QuadTool::QuadInstance) * GizmosData::MaxQuadInstances, BufferUsage::Dynamic }
		});
		s_Data.quads.vertexArray->SetIndexBuffer(IndexBuffer::Create(indices, std::size(indices)));

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

		s_Data.transformBuffer->Bind();

		HL_PROFILE_BEGIN("Gizmos Renderer - Quads");
		if (s_Data.quads.quadInstanceIndex > 0)
		{
			s_Data.quads.vertexArray->GetVertexBuffers()[1]->SetData(s_Data.quads.quadInstances, sizeof(GizmosData::QuadTool::QuadInstance) * s_Data.quads.quadInstanceIndex);

			s_Data.quad_gizmos_shader->Bind();
			s_Data.quads.vertexArray->Bind();

			Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexedInstanced(6u, s_Data.quads.quadInstanceIndex, 0u, 0u, 0u);
			s_Data.quads.quadInstanceIndex = 0u;

			s_Data.quad_gizmos_shader->Unbind();
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
				
				Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexedInstanced(s_Data.m_GizmosInstanceData.m_GizmosObjects[i].m_IndexBuffer->GetCount(), instances, 0u, 0u, 0u);
				instances = 0u;
				s_Data.standard_gizmos_shader->Unbind();
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
			s_Data.transformBuffer->Bind();

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

	void GizmosRenderer::DrawTriangle(Vector3 a, Vector3 b, Vector3 c, Color color, int32_t id)
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

	void GizmosRenderer::DrawLine(Vector3 a, Vector3 b, float width, Color color, int32_t id, LineMode mode)
	{
		if ((s_Data.m_lines.m_index - s_Data.m_lines.m_Lines) >= GizmosData::MaxLineGizmos - 1)
			Flush();

		*s_Data.m_lines.m_index = LineGizmos{
			a, color, (int32_t)mode, width, id
		};
		s_Data.m_lines.m_index++;
		*s_Data.m_lines.m_index = LineGizmos{
			b, color, (int32_t)mode, width, id
		};
		s_Data.m_lines.m_index++;
	}

	void GizmosRenderer::SubmitLine(Vector2 a, Vector2 b, float width, Color color, int32_t id, LineMode mode)
	{
		
	}

	void GizmosRenderer::DrawQuad(EditorCamera camera, TransformComponent& transform, const Vector3& position, const Vector2& size, const Color& color, int data = -1)
	{
		if(s_Data.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances)
			Flush();

		HL_PROFILE_BEGIN("GizmosRenderer - (DrawQuad) Calculate Matrix");
		s_Data.quads.quadInstances[s_Data.quads.quadInstanceIndex] = {
			Matrix4x4::Transpose(
				Matrix4x4::Scale(size) *
				Matrix4x4::RotationRow(
					(Quaternion::Conjugate(transform.Rotation)) * camera.GetTransformComponent().Rotation
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
		Matrix4x4 vertexMatrix = scale * Matrix4x4::RotationRow(Quaternion::Conjugate(transform.Rotation) * camera.GetOrientation());
		Matrix4x4 worldMatrix = Matrix4x4::RotationRow(transform.Rotation) * Matrix4x4::TranslationRow(transform.Position);

		int i = 0;
		for(auto& v : vertices)
		{
			if (s_Data.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances)
				Flush();

			s_Data.quads.quadInstances[s_Data.quads.quadInstanceIndex] = {
				Matrix4x4::Transpose(
					vertexMatrix *
					Matrix4x4::TranslationRow(v.position * transform.Scale) *
					worldMatrix
				),
				Color::Yellow, i
			};
			s_Data.quads.quadInstanceIndex++;
			i++;
		}
	}

	void GizmosRenderer::DrawTool(Transform transform, EditorCamera camera, ToolType type, Tool operation)
	{
		TransformComponent transform_component = transform.GetWorldTransformCache();
		Matrix4x4 transform_matrix = Matrix4x4::TranslationColumn(transform_component.Position) * Matrix4x4::RotationColumn(transform_component.Rotation);

		switch (type)
		{
		case ToolType::Rotate:
		{
			if (*s_Data.m_GizmosInstanceData.TorusGizmoInstanceCount >= GizmosData::MaxGizmosInstances - 4)
				Flush();

			GizmosInstance x[] = {
				{ { }, Color::Red, (int32_t)Tool::RotateX },
				{ { }, Color::Blue, (int32_t)Tool::RotateY },
				{ { }, Color::Green, (int32_t)Tool::RotateZ },
				{ { }, Color::White, (int32_t)Tool::RotateXYZ },
			};

			x[1].transform = transform_matrix;
			x[0].transform = transform_matrix * Matrix4x4::RotationZRow(H_PI_2);
			x[2].transform = transform_matrix * Matrix4x4::RotationXRow(H_PI_2);
			x[3].transform = Matrix4x4::TranslationColumn(transform_component.Position) * Matrix4x4::RotationColumn(Quaternion::Conjugate(transform_component.Rotation) * camera.GetOrientation()) * Matrix4x4::RotationXRow(H_PI_2) * Matrix4x4::Scale(1.5f, 1.5f, 1.5f);

			GizmosInstance* ptr = &s_Data.m_GizmosInstanceData.m_GizmosInstances[2][*s_Data.m_GizmosInstanceData.TorusGizmoInstanceCount];

			memcpy(ptr, x, sizeof(x));
			(*s_Data.m_GizmosInstanceData.TorusGizmoInstanceCount) += std::size(x);

			break;
		}
		case ToolType::Move:
		{
			static GizmosData::QuadTool::QuadInstance xx[] = {
				{ { }, Color::Red, (int32_t)Tool::TranslateZY },
				{ { }, Color::Red, (int32_t)Tool::TranslateZY },
				{ { }, Color::Green, (int32_t)Tool::TranslateXY },
				{ { }, Color::Green, (int32_t)Tool::TranslateXY },
				{ { }, Color::Blue, (int32_t)Tool::TranslateXZ },
				{ { }, Color::Blue, (int32_t)Tool::TranslateXZ },
			};

			if (*s_Data.m_GizmosInstanceData.ArrowGizmoInstanceCount >= GizmosData::MaxGizmosInstances - 3)
				Flush();

			if (s_Data.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances - std::size(xx) - 5)
				Flush();

			GizmosInstance x[4] = {
				{ { }, operation == Tool::TranslateY ? Color::Yellow : Color::Blue, (int32_t)Tool::TranslateY },
				{ { }, operation == Tool::TranslateX ? Color::Yellow : Color::Red, (int32_t)Tool::TranslateX },
				{ { }, operation == Tool::TranslateZ ? Color::Yellow : Color::Green, (int32_t)Tool::TranslateZ },
			};

			x[0].transform = transform_matrix;
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

			xx[0].position = transform_matrix * xx1;
			xx[1].position = transform_matrix * xy1;

			xx[2].position = transform_matrix * xx2;
			xx[3].position = transform_matrix * xy2;

			xx[4].position = transform_matrix * xx3;
			xx[5].position = transform_matrix * xy3;

			memcpy(&s_Data.quads.quadInstances[s_Data.quads.quadInstanceIndex], xx, sizeof(xx));
			s_Data.quads.quadInstanceIndex += std::size(xx);
			
			s_Data.m_GizmosInstanceData.m_GizmosInstances[1][*s_Data.m_GizmosInstanceData.SphereGizmoInstanceCount] = { transform_matrix * Matrix4x4::Scale(0.1f), operation == Tool::TranslateXYZ ? Color::Yellow : Color::White, (int32_t)Tool::TranslateXYZ};
			(*s_Data.m_GizmosInstanceData.SphereGizmoInstanceCount)++;

			break;
		}
		case ToolType::Scale:
		{
			static GizmosData::QuadTool::QuadInstance xx[] = {
				{ { }, Color::Red, (int32_t)Tool::ScaleZY },
				{ { }, Color::Red, (int32_t)Tool::ScaleZY },
				{ { }, Color::Green, (int32_t)Tool::ScaleXY },
				{ { }, Color::Green, (int32_t)Tool::ScaleXY },
				{ { }, Color::Blue, (int32_t)Tool::ScaleXZ },
				{ { }, Color::Blue, (int32_t)Tool::ScaleXZ },
			};

			if (*s_Data.m_GizmosInstanceData.ArrowGizmoInstanceCount >= GizmosData::MaxGizmosInstances - 3)
				Flush();

			if (s_Data.quads.quadInstanceIndex >= GizmosData::MaxQuadInstances - std::size(xx) - 5)
				Flush();

			GizmosInstance x[4] = {
				{ { }, operation == Tool::ScaleY ? Color::Yellow : Color::Blue, (int32_t)Tool::ScaleY },
				{ { }, operation == Tool::ScaleX ? Color::Yellow : Color::Red, (int32_t)Tool::ScaleX },
				{ { }, operation == Tool::ScaleZ ? Color::Yellow : Color::Green, (int32_t)Tool::ScaleZ },
			};

			x[0].transform = transform_matrix;
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

			xx[0].position = transform_matrix * xx1;
			xx[1].position = transform_matrix * xy1;

			xx[2].position = transform_matrix * xx2;
			xx[3].position = transform_matrix * xy2;

			xx[4].position = transform_matrix * xx3;
			xx[5].position = transform_matrix * xy3;

			memcpy(&s_Data.quads.quadInstances[s_Data.quads.quadInstanceIndex], xx, sizeof(xx));
			s_Data.quads.quadInstanceIndex += std::size(xx);
			
			s_Data.m_GizmosInstanceData.m_GizmosInstances[1][*s_Data.m_GizmosInstanceData.SphereGizmoInstanceCount] = { transform_matrix * Matrix4x4::Scale(0.1f), operation == Tool::ScaleXYZ ? Color::Yellow : Color::White, (int32_t)Tool::ScaleXYZ};
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
