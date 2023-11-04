#include "Renderer.h"

#include "Helios/Core/Profiler.h"
#include "Helios/Math/Matrix.h"
#include "Helios/Resources/Shader.h"
#include "Helios/Resources/ShaderBuilder.h"

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

		Ref<UniformBuffer<CBD>> transformBuffer;
		Ref<UniformBuffer<LightData>> lightBuffer;

		Ref<Texture2D> whiteTexture;
		Ref<Material> default_material;

		struct Renderable
		{
			Ref<Mesh> mesh;
			Ref<Material> material;
			Matrix4x4 transform;
			uint32_t entityId;
			uint32_t sceneIndex;
			Color color;
		};
		
		struct InstancedRenderable
		{
			Matrix4x4 wrdViewProj;
			Matrix4x4 wrdProj;
			Color color;
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

		static constexpr uint32_t MAX_INSTANCES = 50000;
		//static constexpr uint32_t MAX_INSTANCES = 1024;
		Ref<DepricatedVertexBuffer> instancedBuffer;
		std::vector<Renderable> renderables;
	};

	void merge(std::vector<RendererData::Renderable>& lA, std::vector<RendererData::Renderable>& rA, std::vector<RendererData::Renderable>& x);
	void merge(RendererData::Renderable* lA, RendererData::Renderable* rA, RendererData::Renderable* arr, uint32_t n);
	
	void mergeSortRenderablesNative(RendererData::Renderable* arr, int const l, int const r);
	void mergeSortRenderablesThreaded(RendererData::Renderable* arr, int const l, int const r, int depth = 0);

	void mergeSortRenderables(std::vector<RendererData::Renderable>& x)
	{
		//mergeSortRenderables(x.data(), 0, x.size() - 1);
		mergeSortRenderablesThreaded(x.data(), 0, x.size() - 1);
	}

	void merge(RendererData::Renderable* arr, int const l, int const mid, int const r)
	{
		int const leftLength = mid - l + 1;
		int const rightLength = r - mid;

		auto left = new RendererData::Renderable[leftLength];
		auto right = new RendererData::Renderable[rightLength];

		for (int i = 0; i < leftLength; i++)
			left[i] = arr[l + i];
		for (int j = 0; j < rightLength; j++)
			right[j] = arr[mid + 1 + j];

		int i = 0;
		int j = 0;
		int k = l;

		while (i < leftLength && j < rightLength)
		{
			if (left[i].material < right[j].material || (left[i].material == right[j].material && left[i].mesh < right[j].mesh))
			{
				arr[k] = left[i];
				i++;
			}
			else
			{
				arr[k] = right[j];
				j++;
			}
			k++;
		}

		while (i < leftLength)
		{
			arr[k] = left[i];
			i++;
			k++;
		}

		while (j < rightLength)
		{
			arr[k] = right[j];
			j++;
			k++;
		}

		delete[] left;
		delete[] right;
	}

	void mergeSortRenderablesNative(RendererData::Renderable* arr, int const l, int const r)
	{
		if (l >= r) return;

		int mid = l + (r - l) / 2;
		mergeSortRenderablesNative(arr, l, mid);
		mergeSortRenderablesNative(arr, mid + 1, r);
		merge(arr, l, mid, r);
	}

	void mergeSortRenderablesThreaded(RendererData::Renderable* arr, int const l, int const r, int depth)
	{
		if (l >= r) return;
		
		int mid = l + (r - l) / 2;

		if ( mid - l > 500 && depth < 4)
		{
			if (depth > 1)
			{
				std::thread leftThread(mergeSortRenderablesNative, arr, l, mid);
				std::thread rightThread(mergeSortRenderablesNative, arr, mid + 1, r);
				leftThread.join();
				rightThread.join();
			}
			else
			{
				std::thread leftThread(mergeSortRenderablesThreaded, arr, l, mid, depth + 1);
				std::thread rightThread(mergeSortRenderablesThreaded, arr, mid + 1, r, depth + 1);
				leftThread.join();
				rightThread.join();
			}
		}
		else
		{
			mergeSortRenderablesNative(arr, l, mid);
			mergeSortRenderablesNative(arr, mid + 1, r);
		}
		
		merge(arr, l, mid, r);
	}

	void merge(RendererData::Renderable* lA, RendererData::Renderable* rA, RendererData::Renderable* arr, uint32_t n)
	{
		int leftsize =  n / 2;
		int rightsize = n - leftsize;
		int i = 0, l = 0, r = 0;

		while (l < leftsize && r < rightsize)
		{
			if (lA[l].material < rA[r].material || (lA[l].material == rA[r].material && lA[l].mesh < rA[r].mesh))
			{
				arr[i] = lA[l];
				i++;
				l++;
			}
			else
			{
				arr[i] = rA[r];
				i++;
				r++;
			}
		}

		while (l < leftsize)
		{
			arr[i] = lA[l];
			i++;
			l++;
		}
		while (r < rightsize)
		{
			arr[i] = rA[r];
			i++;
			r++;
		}
	}

	RendererData rendererData;

	void Renderer::SubmitMesh(uint64_t entityId, int32_t sceneIndex, Matrix4x4 worldMatrix,
		MeshRendererComponent& meshRenderer)
	{
		rendererData.renderables.emplace_back(meshRenderer.mesh, meshRenderer.material, worldMatrix, (uint32_t)entityId, sceneIndex, meshRenderer.material->Color);
	}

	void Renderer::Flush()
	{
		HL_PROFILE_BEGIN("Renderer::Flush");
		// merge sort renderables by material, shader and mesh
		
		ShaderBuilder shaderBuilder;
		shaderBuilder.SetName("StandardInstancedShader");
		shaderBuilder.SetVertexShader("Shaders/StandardInstancedVertexShader");
		shaderBuilder.SetPixelShader("Shaders/StandardInstancedPixelShader");

		SafeInputLayouts<2> inputLayouts({
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

		shaderBuilder.SetInputLayouts(inputLayouts);

		static Ref<Shader> shader = shaderBuilder.Create();

		shader->Bind();

		/*HL_PROFILE_BEGIN("Renderer::Flush::Sort");
		mergeSortRenderables(rendererData.renderables);
		HL_PROFILE_END();*/

		HL_PROFILE_BEGIN("Renderer::Flush::Sort");
		// sort renderables by material, shader and mesh

		std::sort (rendererData.renderables.begin(), rendererData.renderables.end(), [](const RendererData::Renderable& a, const RendererData::Renderable& b) -> bool
		{
			if (a.material < b.material)
				return true;
			else if (a.material == b.material)
			{
				if (a.mesh < b.mesh)
					return true;
			}
			return false;
		});

		HL_PROFILE_END();

		static RendererData::InstancedRenderable* instancedRenderables = new RendererData::InstancedRenderable[RendererData::MAX_INSTANCES];
		
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

		auto test = sizeof(RendererData::InstancedRenderable);

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
			
			*instancedRenderablesPtr++ = RendererData::InstancedRenderable{ rendererData.projectionMatrix * renderable.transform, renderable.transform, renderable.color, renderable.entityId, renderable.sceneIndex };

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
		HL_PROFILE_END();
	}

	bool Renderer::Init()
	{
		rendererData.transformBuffer = UniformBuffer<RendererData::CBD>::Create(0u);
		rendererData.lightBuffer = UniformBuffer<RendererData::LightData>::Create(1u);
		rendererData.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		rendererData.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		rendererData.default_material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
		rendererData.default_material->texture = rendererData.whiteTexture;

		rendererData.instancedBuffer = DepricatedVertexBuffer::Create(sizeof(RendererData::InstancedRenderable) * RendererData::MAX_INSTANCES, BufferUsage::Dynamic);
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
				transform.Up(),
				light.intensity,
				light.color,
			};

			light_data.directional_light_count++;
		}

		rendererData.lightBuffer->SetData(light_data);
		rendererData.lightBuffer->Bind();
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

		static Ref<Shader> shader = Shader::Create("Standard", "StandardVertexShader", "StandardPixelShader", InputLayouts {
			InputLayout {
				{ "Position", ShaderDataType::Float32_3 },
				{ "TexCoord", ShaderDataType::Float32_2 },
				{ "Normal",   ShaderDataType::Float32_3 }
			}
		}, DepthFunc::Less, Topology::TriangleList);

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
		rendererData.transformBuffer->Bind();
		HL_PROFILE_END();

		HL_PROFILE_BEGIN("Draw Call");
		Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexed(meshRenderer.mesh->getIndexCount(), 0u, 0u);
		HL_PROFILE_END();
		HL_PROFILE_END();
	}

	
}

