#include "Renderer.h"

#include "Helios/Translation/Matrix.h"
#include "Helios/Resources/Shader.h"

namespace Helios
{
	struct RendererData
	{
		Matrix4x4 projectionMatrix;
		struct CBD { Matrix4x4 transform; Color color; float entityId; float b; float c; float d; };
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

	void Renderer::BeginScene(Matrix4x4 projection)
	{
		rendererData.projectionMatrix = projection;
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::DrawMesh(uint32_t entityId, Components::Transform& transform, Components::MeshRenderer& meshRenderer)
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
				rendererData.projectionMatrix* // Projection is column major
				Matrix4x4::TranslationColumn(transform.position)*
				Matrix4x4::RotationColumn(transform.rotation)*
				Matrix4x4::Scale(transform.scale)
			},
			meshRenderer.material->Color,
			(float)entityId, 0, 0, 0
		};


		rendererData.transformBuffer->SetData(&cb, sizeof(cb));
		rendererData.transformBuffer->Bind(0);


		Graphics::instance->m_deviceContext->DrawIndexed(meshRenderer.mesh->GetIndexCount(), 0u, 0u);
	}
}

