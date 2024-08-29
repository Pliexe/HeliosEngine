#pragma once
#include "Helios/Resources/Material.h"
#include "Helios/Resources/Mesh.h"

namespace Helios
{
	struct HELIOS_API SpriteRendererComponent
	{
		Color color;
		Ref<Texture2D> texture;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const Color& color) : color(color) { }
	};

	struct HELIOS_API MeshRendererComponent
	{
		Ref<Mesh> mesh;
		Ref<Material> material;

		bool castShadow = true;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(const Ref<Mesh>& mesh) : mesh(mesh) { }
	};
}
