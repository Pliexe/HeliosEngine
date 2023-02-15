#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Resources/Color.h"
#include "Helios/Scene/Components.h"
#include "Helios/Scene/SceneCamera.h"
#include "Helios/Translation/Matrix.h"
#include "Helios/Utils/MeshBuilder.h"

namespace Helios
{
	class HELIOS_API GizmosRenderer
	{
	private:
		static Ref<std::vector<MeshVertex>> m_DisplayedVertices;
	public:
		static bool Init();
		static void Begin(Matrix4x4 projection);
		static void End();
		static void RenderVertices();
		static void Flush();
		static void DrawQuad(SceneCamera camera, Components::Transform& transform, const Vector3& position, const Vector2& size, const Color& color, float
		                     data);
		static void DrawMeshVertices(SceneCamera camera, Components::Transform& transform, std::vector<MeshVertex>& vertices);

		static void SetDisplayedVertices(Ref<std::vector<MeshVertex>> vertices);
		static void ClearDisplayedVertices();
	};
}
