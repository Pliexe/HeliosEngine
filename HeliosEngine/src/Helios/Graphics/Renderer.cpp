#include "Renderer.h"

#include "Helios/Translation/Matrix.h"
#include "Helios/Resources/Shader.h"

namespace Helios
{
	struct RendererData
	{
		Matrix4x4 projectionMatrix;
		struct CBD { Matrix4x4 transform; Color color; };
		Ref<ConstantBuffer> transformBuffer;

		Ref<Texture2D> whiteTexture;
	};

	RendererData rendererData;

	bool Renderer::Init()
	{
		rendererData.transformBuffer = ConstantBuffer::Create(sizeof(RendererData::CBD));
		rendererData.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		rendererData.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		return true;
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::BeginScene(Components::Transform& trans, Components::Camera& cam)
	{
		auto size = Graphics::GetCurrentSize();

		rendererData.projectionMatrix = (
			Matrix4x4::Translation(-trans.position) *
			(
				Matrix4x4::RotationColumn(trans.rotation)
			) *
			(cam.ortographic ? (
				Matrix4x4::OrthographicLH(cam.size, cam.size * ((float)size.y / (float)size.x), cam.near_z, cam.far_z)
			) : (
				Matrix4x4::PerspectiveLH(cam.fov * 3.14f / 180.0f, ((float)size.x / (float)size.y), cam.near_z, cam.far_z)
			))
		);
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::DrawMesh(Components::Transform& transform, Components::MeshRenderer& meshRenderer)
	{
		static Ref<Shader> shader = CreateRef<Shader>(Shader("Standard", {
			{ "Position", Shader::DataType::Float3 },
			{ "TexCoord", Shader::DataType::Float2 }
		}));

		shader->Bind();
		meshRenderer.mesh->Bind();
		if (meshRenderer.material->texture == nullptr)
			rendererData.whiteTexture->Bind(0u);
		else
			meshRenderer.material->Bind(0u);

		const RendererData::CBD cb =
		{
			{
				Matrix4x4::Transpose(
					Matrix4x4::Scale(transform.scale) *
					Matrix4x4::Rotation(transform.rotation) *
					Matrix4x4::Translation(transform.position) *
					rendererData.projectionMatrix
				)
			},
			meshRenderer.material->Color
		};


		rendererData.transformBuffer->SetData(&cb, sizeof(cb));
		rendererData.transformBuffer->Bind(0);


		Graphics::instance->m_deviceContext->DrawIndexed(meshRenderer.mesh->GetIndexCount(), 0u, 0u);
	}
}

