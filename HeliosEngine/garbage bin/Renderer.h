#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Resources/Mesh.h"
#include "Helios/Scene/Components.h"
#include "Helios/Math/Matrix.h"
#include "Helios/Scene/EntityContainer.h"

namespace Helios
{
	class Framebuffer;
	class Renderer
	{
	public:
		static bool Init();
		static void Shutdown();
		static void BeginScene(Ref<Framebuffer>& colorBuffer,
		                       Matrix4x4 projection, Vector3 cam_pos,
		                       Color ambient_light, entt::registry& container);

		static void BeginScene(Matrix4x4 projection);
		static void EndScene();
		static void Flush();

		static void DrawMesh(uint64_t entityId, int32_t sceneIndex, Matrix4x4 worldMatrix, MeshRendererComponent& meshRenderer);
		static void SubmitMesh(uint64_t entityId, int32_t sceneIndex, Matrix4x4 worldMatrix, MeshRendererComponent& meshRenderer);

	};
}
