#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Resources/Mesh.h"
#include "Helios/Scene/Components.h"

namespace Helios
{
	class Renderer
	{
	public:

		static bool Init();
		static void Shutdown();

		static void BeginScene(Components::Transform& trans, Components::Camera& cam);
		static void EndScene();

		static void DrawMesh(Components::Transform& transform, Components::MeshRenderer& meshRenderer);

	};
}