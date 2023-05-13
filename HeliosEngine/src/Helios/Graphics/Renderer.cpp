#include "Renderer.h"

#include "Helios/Translation/Matrix.h"
#include "Helios/Resources/Shader.h"

namespace Helios
{
	struct RendererData
	{
		Matrix4x4 projectionMatrix;
		struct CBD { Matrix4x4 wvpProjection; Matrix4x4 worldProjection; Color color; float entityId; float b; float c; float d; };
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
	};

	RendererData rendererData;

	bool Renderer::Init()
	{
		rendererData.transformBuffer = ConstantBuffer<RendererData::CBD>::Create();
		rendererData.lightBuffer = ConstantBuffer<RendererData::LightData>::Create();
		rendererData.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		rendererData.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		rendererData.default_material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
		rendererData.default_material->texture = rendererData.whiteTexture;
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
	}

	void Renderer::DrawMesh(uint32_t entityId, Matrix4x4 worldMatrix, MeshRendererComponent& meshRenderer)
	{
		static Ref<Shader> shader = CreateRef<Shader>(Shader("Standard", {
			{ "Position", Shader::DataType::Float3 },
			{ "TexCoord", Shader::DataType::Float2 },
			{ "Normal",   Shader::DataType::Float3 },
		}));

		shader->Bind();
		meshRenderer.mesh->Bind();
		if (meshRenderer.material->texture == nullptr)
		{
			rendererData.default_material->Bind(0u);
		}
		else
			meshRenderer.material->Bind(0u);

		const RendererData::CBD cb =
		{
			rendererData.projectionMatrix * worldMatrix,
			worldMatrix,
			meshRenderer.material->Color,
			(float)entityId, 0, 0, 0
		}; 

		rendererData.transformBuffer->SetData(cb);
		rendererData.transformBuffer->BindVS(0);


		Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexed(meshRenderer.mesh->getIndexCount(), 0u, 0u);
	}
}

