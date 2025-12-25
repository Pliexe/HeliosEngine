#include "Renderer.h"

#include "Helios/Core/Profiler.h"
#include "Helios/Math/Matrix.h"
#include "../Resources/DepricatedShader.h"
#include "Helios/Resources/ShaderBuilder.h"
#include "Framebuffer.h"
#include "GizmosRenderer.h"

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

		struct DirectionalLightData
		{
			Matrix4x4 lightViewProj;
			// first pack
			Vector3 direction; // 12 bytes
			float intensity; // 4 bytes
			// second pack
			Color color; // 16 bytes
		};

		struct LightData
		{
			Color ambient_light; // 16 bytes
			DirectionalLightData directionalLights[4]; // 32b * 4 bytes > 128 bytes
			uint32_t directional_light_count; // 4 bytes
		};

		Ref<UniformBuffer<CBD>> transformBuffer;
		Ref<UniformBuffer<LightData>> lightBuffer;

		RendererData::LightData lightData;
		Ref<Framebuffer> directinalLightShadowFramebuffers[4];

		Ref<Texture2D> whiteTexture;
		Ref<Material> default_material;

		Ref<Framebuffer> currentFramebuffer;
		Vector3 cam_pos = Vector3::Zero();

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

		std::vector<Renderable> renderables;
		std::vector<bool> castShadowMesh;
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
		if (meshRenderer.mesh == nullptr || meshRenderer.material == nullptr) return;
		rendererData.renderables.emplace_back(meshRenderer.mesh, meshRenderer.material, worldMatrix, (uint32_t)entityId, sceneIndex, meshRenderer.material->Color);
		rendererData.castShadowMesh.emplace_back(meshRenderer.castShadow);
	}

	static Ref<DepricatedShader> CreateShadowShader()
	{
		ShaderBuilder shaderBuilder;
		shaderBuilder.SetName("ShadowShader");
		shaderBuilder.SetVertexShader("CompiledShaders/ShadowInstancedVertexShader");
		shaderBuilder.SetPixelShader("CompiledShaders/ShadowInstancedPixelShader");
		shaderBuilder.SetDepthFunc(DepthFunc::LessEqual);

		const SafeInputLayouts<2> inputLayouts = Mesh::GetInputLayout();

		shaderBuilder.SetInputLayouts(inputLayouts);

		return shaderBuilder.Create();
	}

	void Renderer::Flush()
	{
		HL_PROFILE_BEGIN("Renderer::Flush");
		// merge sort renderables by material, shader and mesh

		ShaderBuilder shaderBuilder;
		shaderBuilder.SetName("StandardInstancedShader");
		shaderBuilder.SetVertexShader("CompiledShaders/StandardInstancedVertexShader");
		shaderBuilder.SetPixelShader("CompiledShaders/StandardInstancedPixelShader");

		const SafeInputLayouts<2> inputLayouts = Mesh::GetInputLayout();

		shaderBuilder.SetInputLayouts(inputLayouts);

		static Ref<DepricatedShader> shader = shaderBuilder.Create();

		static Ref<DepricatedShader> shadowShader = CreateShadowShader();

		/*HL_PROFILE_BEGIN("Renderer::Flush::Sort");
		mergeSortRenderables(rendererData.renderables);
		HL_PROFILE_END();*/

		HL_PROFILE_BEGIN("Renderer::Flush::Sort");
		// sort renderables by material, shader and mesh


		/*std::sort (rendererData.renderables.begin(), rendererData.renderables.end(), [](const RendererData::Renderable& a, const RendererData::Renderable& b) -> bool
		{
			if (a.material < b.material)
				return true;
			else if (a.material == b.material)
			{
				if (a.mesh < b.mesh)
					return true;
			}
			return false;
		});*/

		HL_PROFILE_END();




		static RendererData::InstancedRenderable* instancedRenderables = new RendererData::InstancedRenderable[Mesh::GetMaxInstanceCount()];


		RendererData::InstancedRenderable* instancedRenderablesPtr = instancedRenderables;

		Ref<Material> currentMaterial = nullptr;
		Ref<Mesh> currentMesh = nullptr;

		auto draw = [&]()
		{
			uint32_t size = (instancedRenderablesPtr - instancedRenderables);

			Mesh::GetInstanceVertexBuffer()->SetData(instancedRenderables, size * sizeof(RendererData::InstancedRenderable));

			#ifdef HELIOS_PLATFORM_WINDOWS
			Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexedInstanced(currentMesh->getIndexCount(), size, 0u, 0u, 0u);
			#endif

			instancedRenderablesPtr = instancedRenderables;
		};

		// Shadow map pass

		shader->Unbind();
		shadowShader->Bind();


		for (uint32_t i = 0, n = rendererData.lightData.directional_light_count; i < n; i++)
		{
			rendererData.directinalLightShadowFramebuffers[i]->ClearDepthStencil();
			rendererData.directinalLightShadowFramebuffers[i]->Bind();

			uint32_t j = 0;
			for (auto& renderable : rendererData.renderables)
			{
				if (!rendererData.castShadowMesh[j])
				{
					j++;
					continue;
				}

				if (currentMesh != renderable.mesh)
				{
					if ((instancedRenderablesPtr - instancedRenderables) > 0) {
						draw();
					}

					if (currentMesh == nullptr)
					{
						currentMesh = renderable.mesh;
						currentMesh->Bind();
					}

					currentMesh = renderable.mesh;
					currentMesh->Bind();
				}

				RendererData::InstancedRenderable irptrtest = RendererData::InstancedRenderable{ rendererData.lightData.directionalLights[i].lightViewProj * renderable.transform, renderable.transform, renderable.color, renderable.entityId, renderable.sceneIndex };

				*instancedRenderablesPtr++ = irptrtest;

				if ((instancedRenderablesPtr - instancedRenderables) >= Mesh::GetMaxInstanceCount())
				{
					draw();
				}
				j++;
			}

			if (instancedRenderablesPtr != instancedRenderables)
			{
				draw();
			}

			rendererData.directinalLightShadowFramebuffers[i]->Unbind();
		}

		shadowShader->Unbind();

		// Render objects pass
		static Ref<Material> shadowMaterial = Material::Create(Material::Filter::ComparisonMinMagMipLinear, Material::Type::Border);
		shadowMaterial->Bind(1u);

		rendererData.currentFramebuffer->Bind();
		shader->Bind();

		for (uint32_t i = 0, n = rendererData.lightData.directional_light_count; i < n; i++)
		{
			auto shadowMap = rendererData.directinalLightShadowFramebuffers[i];
			shadowMap->BindDepth(5u + i);
		}

		instancedRenderablesPtr = instancedRenderables;

		currentMaterial = nullptr;
		currentMesh = nullptr;

		for (auto& renderable : rendererData.renderables)
		{
			if (currentMaterial != renderable.material)
			{
				if ((instancedRenderablesPtr - instancedRenderables) > 0) {
					draw();
				}

				currentMaterial = renderable.material;
				currentMaterial->Unbind();
				currentMaterial->Bind(0u);

				if (currentMaterial->texture == nullptr)
				{
					rendererData.default_material->Bind(0u);
				}
			}

			if (currentMesh != renderable.mesh)
			{
				if ((instancedRenderablesPtr - instancedRenderables) > 0) {
					draw();
				}

				if (currentMesh == nullptr)
				{
					currentMesh = renderable.mesh;
					currentMesh->Bind();
				}

				currentMesh = renderable.mesh;
				currentMesh->Bind();
			}

			RendererData::InstancedRenderable irptrtest = RendererData::InstancedRenderable{ rendererData.projectionMatrix * renderable.transform, renderable.transform, renderable.color, renderable.entityId, renderable.sceneIndex };

			*instancedRenderablesPtr++ = irptrtest;
			//*instancedRenderablesPtr++ = RendererData::InstancedRenderable{ rendererData.projectionMatrix * renderable.transform, renderable.transform, renderable.color, renderable.entityId, renderable.sceneIndex };

			if ((instancedRenderablesPtr - instancedRenderables) >= Mesh::GetMaxInstanceCount())
			{
				draw();
			}
		}

		if (instancedRenderablesPtr != instancedRenderables)
		{
			draw();
		}

		for (uint32_t i = 0, n = rendererData.lightData.directional_light_count; i < n; i++)
		{
			auto shadowMap = rendererData.directinalLightShadowFramebuffers[i];
			shadowMap->UnbindDepth(5u + i);
		}

		shadowMaterial->Unbind();
		shader->Unbind();
		rendererData.currentFramebuffer->Unbind();

		rendererData.renderables.clear();
		rendererData.castShadowMesh.clear();
		HL_PROFILE_END();
	}

	bool Renderer::Init()
	{
		rendererData.transformBuffer = UniformBuffer<RendererData::CBD>::Create(0u);
		rendererData.lightBuffer = UniformBuffer<RendererData::LightData>::Create(1u);
		rendererData.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		rendererData.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		rendererData.default_material = Material::Create(Material::Filter::MinMagMipPoint, Material::Type::Warp);
		rendererData.default_material->texture = rendererData.whiteTexture;

		return true;
	}

	void Renderer::Shutdown()
	{
		rendererData.transformBuffer.reset();
		rendererData.lightBuffer.reset();
		rendererData.directinalLightShadowFramebuffers->reset();
		rendererData.whiteTexture.reset();
		rendererData.default_material.reset();
		rendererData.currentFramebuffer.reset();

		for (auto& renderable : rendererData.renderables)
		{
			renderable.mesh.reset();
			renderable.material.reset();
		}
	}

	void Renderer::BeginScene(Ref<Framebuffer>& colorBuffer, Matrix4x4 projection, Vector3 cam_pos, Color ambient_light, entt::registry& container)
	{
		rendererData.currentFramebuffer = colorBuffer;
		rendererData.cam_pos = cam_pos;
		std::memset(static_cast<void*>(&rendererData.lightData), 0, sizeof(RendererData::LightData));
		//lightData.directional_light_count = 0;
		rendererData.lightData.ambient_light = ambient_light;
		auto view = container.view<TransformComponent, DirectionalLightComponent>(entt::exclude<DisabledObjectComponent>);
		for (auto entity : view)
		{
			if (rendererData.lightData.directional_light_count > 3)
			{
				//HL_CORE_WARN("Only 4 directional lights are supported!");
				break;
			}

			auto [transform, light] = view.get<TransformComponent, DirectionalLightComponent>(entity);

			if (light.framebuffer == nullptr)
			{
				light.framebuffer =
					Framebuffer::Create(1024, 1024,
						{ Framebuffer::Format::D32F }
				);
			}

			rendererData.directinalLightShadowFramebuffers[rendererData.lightData.directional_light_count] = light.framebuffer;


			float light_offset = 10.0f;
			Vector3 camPos = transform.Position + (light_offset * (-transform.Forward()));

			GizmosRenderer::DrawLine(transform.Position, camPos, 2.0f, Color::Blue, -1, GizmosRenderer::LineMode::Dash_Dot_Dot);

			float near_plane = 1.0f, far_plane = 7.5f;
			static float size = 40.0f;
			static Matrix4x4 ort = Matrix4x4::OrthographicLH(-size / 2.f, size / 2.f, -size / 2.f, size / 2.f, near_plane, far_plane);
			//static Matrix4x4 ort = Matrix4x4::OrthographicLH(size, size, near_plane, far_plane);

			rendererData.lightData.directionalLights[rendererData.lightData.directional_light_count] = {
				ort *
				//Matrix4x4::LookAtLH(Vector3::Zero(), transform.Forward(), Vector3::Up()),
				//Matrix4x4::Inverse(
					(Matrix4x4::Translation(transform.Position) *
					Matrix4x4::Rotation(transform.Rotation)),
				//),
				(-transform.Forward()),
				//transform.Forward(),
				light.intensity,
				light.color,
			};

			rendererData.lightData.directional_light_count++;
		}

		rendererData.lightBuffer->SetData(rendererData.lightData);
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

		static Ref<DepricatedShader> shader = DepricatedShader::Create("Standard", "StandardVertexShader", "StandardPixelShader", InputLayouts {
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
		#ifdef HELIOS_PLATFORM_WINDOWS
		Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexed(meshRenderer.mesh->getIndexCount(), 0u, 0u);
		#endif
		HL_PROFILE_END();
		HL_PROFILE_END();
	}


}
