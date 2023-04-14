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
	class EditorCamera;
	class HELIOS_API GizmosRenderer
	{
	private:
		static Ref<std::vector<MeshVertex>> m_DisplayedVertices;
	public:
		enum class Tool
		{
			None = -1,
			MoveX = -2,
			MoveY = -3,
			MoveZ = -4,
			MoveXY = -5,
			MoveXZ = -6,
			MoveZY = -7,
			MoveXYZ = -8,
			ScaleX = -9,
			ScaleY = -10,
			ScaleZ = -11,
			ScaleXYZ = -12,
			RotateX = -13,
			RotateY = -14,
			RotateZ = -15,
		};

		enum class ToolType
		{
			Move,
			Scale,
			Rotate
		};

		~GizmosRenderer();

		static bool Init();
		static void Begin(Matrix4x4 projection);
		static void End();
		static void RenderVertices();
		static void Flush();
		static void DrawLine(Vector3 a, Vector3 b, float width = 0.5f, Color color = Color::White, int64_t id = -1, int32_t mode = -1);
		static void DrawQuad(SceneCamera camera, TransformComponent& transform, const Vector3& position, const Vector2& size, const Color& color, float
		                     data);
		static void DrawMeshVertices(EditorCamera camera, TransformComponent& transform, std::vector<MeshVertex>& vertices);

		static void DrawTool(Matrix4x4 transform, ToolType type, Tool operation = Tool::None);

		static void SetDisplayedVertices(Ref<std::vector<MeshVertex>> vertices);
		static void ClearDisplayedVertices();
	};
}
