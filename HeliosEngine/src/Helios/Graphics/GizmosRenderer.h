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

		enum class LineMode
		{
			Solid = 0,
			Dashed = 1,
			Dotted = 2,
			Dash_Dot = 3,
			Dash_Dot_Dot = 4,
			Rounded_Solid = 5,
			Rounded_Dashed = 6,
			Rounded_Dotted = 7,
			Rounded_Dash_Dot = 8,
			Rounded_Dash_Dot_Dot = 9,
		};

		~GizmosRenderer();

		static bool Init();
		static void Begin(Matrix4x4 projection);
		static void End();
		static void RenderVertices();
		static void Flush();
		static void DrawTriangle(Vector3 a, Vector3 b, Vector3 c, Color color, int32_t id);
		static void DrawAngle(Vector3 a_normal, Vector3 b_normal, float radius, Color color, Matrix4x4 model_matrix);
		static void DrawAngle(float angle, float radius, Color color, Matrix4x4 model_matrix);
		static void DrawAngle(Vector3 root, Vector3 a_normal, Vector3 b_normal, float radius, Color color, Matrix4x4 model_matrix);
		static void DrawLine(Vector3 a, Vector3 b, float width = 0.5f, Color color = Color::White, int32_t id = -1, LineMode mode = LineMode::Solid);
		static void DrawQuad(SceneCamera camera, TransformComponent& transform, const Vector3& position, const Vector2& size, const Color& color, float
		                     data);
		static void DrawMeshVertices(EditorCamera camera, TransformComponent& transform, std::vector<MeshVertex>& vertices);

		static void DrawTool(Matrix4x4 transform, ToolType type, Tool operation = Tool::None);

		static void SetDisplayedVertices(Ref<std::vector<MeshVertex>> vertices);
		static void ClearDisplayedVertices();
	};
}
