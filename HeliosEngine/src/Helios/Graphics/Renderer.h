#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Resources/Mesh.h"
#include "Helios/Scene/Components.h"
#include "Helios/Math/Matrix.h"

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
		static void Flush();

		static void DrawMesh(uint64_t entityId, int32_t sceneIndex, Matrix4x4 worldMatrix, MeshRendererComponent& meshRenderer);
		static void SubmitMesh(uint64_t entityId, int32_t sceneIndex, Matrix4x4 worldMatrix, MeshRendererComponent& meshRenderer);

	};
}