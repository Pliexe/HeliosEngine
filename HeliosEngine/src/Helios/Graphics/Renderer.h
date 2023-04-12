#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Resources/Mesh.h"
#include "Helios/Scene/Components.h"
#include "Helios/Translation/Matrix.h"

namespace Helios
{
	class Renderer
	{
	public:

		static bool Init();
		static void Shutdown();
		static void BeginScene(Matrix4x4 projection,
		                       Color ambient_light, entt::basic_view<entt::entity, entt::get_t<TransformComponent, DirectionalLightComponent>,
		                       entt::
		                       exclude_t<DisabledObjectComponent>> directional_light_view);

		static void BeginScene(Matrix4x4 projection);
		static void EndScene();

		static void DrawMesh(uint32_t entityId, Matrix4x4 worldMatrix, MeshRendererComponent& meshRenderer);

	};
}