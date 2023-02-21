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
		                       entt::basic_view<entt::entity, entt::get_t<Components::Transform, Components::DirectionalLight>, entt::
		                       exclude_t<Components::DisabledObject>> directionalLightView);

		static void BeginScene(Matrix4x4 projection);
		static void EndScene();

		static void DrawMesh(uint32_t entityId, Components::Transform& transform, Components::MeshRenderer& meshRenderer);

	};
}