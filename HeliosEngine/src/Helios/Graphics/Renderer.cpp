#include "Renderer.h"

#include "Helios/Core/Profiler.h"
#include "Helios/Translation/Matrix.h"
#include "Helios/Resources/Shader.h"

namespace Helios
{
	struct RendererData
	{
		Matrix4x4 projectionMatrix;
		struct CBD
		{
			Matrix4x4 wvpProjection;
			Matrix4x4 worldProjection;
			Color color;
			uint64_t entityId;
			int32_t sceneIndex;
		};

		/*struct CBDInstanced
		{
			Color color;
		};*/

		static struct DirectionalLightData
		{
			// first pack
			Vector3 direction; // 12 bytes
			float intensity; // 4 bytes
			// second pack
			Color color; // 16 bytes
		};

		static struct LightData
		{
			Color ambient_light; // 16 bytes
			DirectionalLightData directionalLights[4]; // 32b * 4 bytes > 128 bytes
			uint32_t directional_light_count; // 4 bytes 
		};  

		Ref<ConstantBuffer<CBD>> transformBuffer;
		Ref<ConstantBuffer<LightData>> lightBuffer;

		Ref<Texture2D> whiteTexture;
		Ref<Material> default_material;

		struct Renderable
		{
			Ref<Mesh> mesh;
			Ref<Material> material;
			Matrix4x4 transform;
			uint32_t entityId;
			uint32_t sceneIndex;
		};

		struct InstancedRenderable
		{
			Matrix4x4 wrdViewProj;
			Matrix4x4 wrdProj;
			uint32_t entityId;
			uint32_t sceneIndex;
		};

		struct Batch
		{
			Ref<Mesh> mesh;
			Ref<Material> shader;
			uint64_t offset;
			uint64_t count;
		};

		static constexpr uint32_t MAX_INSTANCES = 1024;
		Ref<VertexBuffer> instancedBuffer;
		std::vector<Renderable> renderables;
	};

	void merge(std::vector<RendererData::Renderable>& lA, std::vector<RendererData::Renderable>& rA, std::vector<RendererData::Renderable>& x);

	/*void mergeSortRenderables(std::vector<RendererData::Renderable>& x)
	{
		
	}*/

	void mergeSortRenderables(std::vector<RendererData::Renderable> &x)
	{
		int length = x.size();
		if (length <= 1) return;

		int mid = length / 2;
		std::vector<RendererData::Renderable> left;
		std::vector<RendererData::Renderable> right;
		left.reserve(mid);
		left.reserve(length - mid);

		int i = 0; // left
		int j = 0; // right

		for (; i < length; i++)
		{
			if (i < mid)
				left.push_back(x[i]);
			else
			{
				right.push_back(x[i]);
				j++;
			}
		}
		mergeSortRenderables(left);
		mergeSortRenderables(right);
		merge(left, right, x);
	}

	void merge(std::vector<RendererData::Renderable>& lA, std::vector<RendererData::Renderable>& rA, std::vector<RendererData::Renderable>& x)
	{
		int leftsize = x.size() / 2;
		int rightsize = x.size() - leftsize;
		int i = 0, l = 0, r = 0;

		while (l < leftsize && r < rightsize)
		{
			if (lA[l].material < rA[r].material || (lA[l].material == rA[r].material && lA[l].mesh < rA[r].mesh))
			{
				x[i] = lA[l];
				i++;
				l++;
			}
			else
			{
				x[i] = rA[r];
				i++;
				r++;
			}
		}

		while (l < leftsize)
		{
			x[i] = lA[l];
			i++;
			l++;
		}
		while (r < rightsize)
		{
			x[i] = rA[r];
			i++;
			r++;
		}
	}	

	RendererData rendererData;

	void Renderer::SubmitMesh(uint64_t entityId, int32_t sceneIndex, Matrix4x4 worldMatrix,
		MeshRendererComponent& meshRenderer)
	{
		RendererData::Renderable renderable;
		renderable.entityId = entityId;
		renderable.sceneIndex = sceneIndex;
		renderable.mesh = meshRenderer.mesh;
		renderable.material = meshRenderer.material;
		renderable.transform = worldMatrix;
		rendererData.renderables.push_back(renderable);
	}

	void Renderer::Flush()
	{
		// merge sort renderables by material, shader and mesh
		static Ref<Shader> shader = CreateRef<Shader>(Shader("StandardInstanced", {
			{ "Position", Shader::DataType::Float3 },
			{ "TexCoord", Shader::DataType::Float2 },
			{ "Normal",   Shader::DataType::Float3 },
			{ "WorldViewProj", Shader::DataType::Matrix4x4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "WorldProj", Shader::DataType::Matrix4x4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "EntityId", Shader::DataType::Int32, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "SceneIndex", Shader::DataType::Int32, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
		}));

		shader->Bind();

		HL_PROFILE_BEGIN("Renderer::Flush::Sort");
		mergeSortRenderables(rendererData.renderables);
		HL_PROFILE_END();

		RendererData::InstancedRenderable instancedRenderables[RendererData::MAX_INSTANCES];
		RendererData::InstancedRenderable* instancedRenderablesPtr = instancedRenderables;

		Ref<Material> currentMaterial = nullptr;
		Ref<Mesh> currentMesh = nullptr;

		auto draw = [&]
		{
			uint32_t size = (instancedRenderablesPtr - instancedRenderables);

			rendererData.instancedBuffer->SetData(instancedRenderables, size * sizeof(RendererData::InstancedRenderable));
			rendererData.instancedBuffer->Bind(1u);

			HL_PROFILE_BEGIN("Draw Call");
			Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexedInstanced(currentMesh->getIndexCount(), size, 0u, 0u, 0u);
			HL_PROFILE_END();

			instancedRenderablesPtr = instancedRenderables;
		};

		for (auto& renderable : rendererData.renderables)
		{
			if (currentMaterial != renderable.material)
			{
				currentMaterial = renderable.material;
				currentMaterial->Bind(0u);

				if (currentMaterial->texture == nullptr)
				{
					rendererData.default_material->Bind(0u);
				}
			}

			if (currentMesh != renderable.mesh)
			{
				if (currentMesh == nullptr)
				{
					currentMesh = renderable.mesh;
					currentMesh->Bind();
				}

				draw();

				currentMesh = renderable.mesh;
				currentMesh->Bind();
			}

			*instancedRenderablesPtr++ = RendererData::InstancedRenderable{ rendererData.projectionMatrix * renderable.transform, renderable.transform, renderable.entityId, renderable.sceneIndex };
			uint32_t size = (instancedRenderablesPtr - instancedRenderables);

 			if ((instancedRenderablesPtr - instancedRenderables) >= RendererData::MAX_INSTANCES)
			{
				draw();
			}
		}

		if (instancedRenderablesPtr != instancedRenderables)
		{
			draw();
		}

		rendererData.renderables.clear();
	}

	bool Renderer::Init()
	{
		rendererData.transformBuffer = ConstantBuffer<RendererData::CBD>::Create();
		rendererData.lightBuffer = ConstantBuffer<RendererData::LightData>::Create();
		rendererData.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		rendererData.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		rendererData.default_material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
		rendererData.default_material->texture = rendererData.whiteTexture;

		rendererData.instancedBuffer = VertexBuffer::Create(sizeof(RendererData::InstancedRenderable) * RendererData::MAX_INSTANCES, BufferUsage::Dynamic);
		rendererData.instancedBuffer->SetStride<RendererData::InstancedRenderable>();
		return true;
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::BeginScene(Matrix4x4 projection, Color ambient_light, entt::basic_view<entt::entity, entt::get_t<TransformComponent, DirectionalLightComponent>, entt::exclude_t<DisabledObjectComponent>> directional_light_view)
	{
		RendererData::LightData light_data;
		ZeroMemory(&light_data, sizeof(RendererData::LightData));
		//lightData.directional_light_count = 0;
		light_data.ambient_light = ambient_light;

		for (auto entity : directional_light_view)
		{
			if (light_data.directional_light_count > 3)
			{
				//HL_CORE_WARN("Only 4 directional lights are supported!");
				break;
			}

			auto [transform, light] = directional_light_view.get<TransformComponent, DirectionalLightComponent>(entity);

			light_data.directionalLights[light_data.directional_light_count] = {
				transform.Forward(),
				light.intensity,
				light.color,
			};

			light_data.directional_light_count++;
		}

		rendererData.lightBuffer->SetData(light_data);
		rendererData.lightBuffer->BindPS(0u);
		rendererData.projectionMatrix = projection;
	}

	void Renderer::BeginScene(Matrix4x4 projection)
	{	
		rendererData.projectionMatrix = projection;
	}

	void Renderer::EndScene()
	{
		// merge sort renderables by material

		Flush();
	}

	void Renderer::DrawMesh(uint64_t entityId, int32_t sceneIndex, Matrix4x4 worldMatrix, MeshRendererComponent& meshRenderer)
	{
		if (meshRenderer.mesh == nullptr || meshRenderer.material == nullptr) return;
		HL_PROFILE_BEGIN("Draw Mesh");
		HL_PROFILE_BEGIN("Create Shader");
		static Ref<Shader> shader = CreateRef<Shader>(Shader("Standard", {
			{ "Position", Shader::DataType::Float3 },
			{ "TexCoord", Shader::DataType::Float2 },
			{ "Normal",   Shader::DataType::Float3 },
		}));
		HL_PROFILE_END();

		HL_PROFILE_BEGIN("Bind shader, mesh, texture");
		shader->Bind();
		meshRenderer.mesh->Bind();
		if (meshRenderer.material->texture == nullptr)
		{
			rendererData.default_material->Bind(0u);
		}
		else
			meshRenderer.material->Bind(0u);
		HL_PROFILE_END();

		HL_PROFILE_BEGIN("Set data");
		const RendererData::CBD cb =
		{
			rendererData.projectionMatrix * worldMatrix,
			worldMatrix,
			meshRenderer.material->Color,
			entityId, sceneIndex
		};

		rendererData.transformBuffer->SetData(cb);
		rendererData.transformBuffer->BindVS(0);
		HL_PROFILE_END();

		HL_PROFILE_BEGIN("Draw Call");
		Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexed(meshRenderer.mesh->getIndexCount(), 0u, 0u);
		HL_PROFILE_END();
		HL_PROFILE_END();
	}

	
}

