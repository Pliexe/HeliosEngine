#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Resources/Color.h"
#include "Helios/Math/Matrix.h"
#include "Helios/Utils/MeshBuilder.h"
#include "Helios/Scene/Entity.h"

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
			TranslateX = -2,
			TranslateY = -3,
			TranslateZ = -4,
			TranslateXY = -5,
			TranslateXZ = -6,
			TranslateZY = -7,
			TranslateXYZ = -8,
			ScaleX = -9,
			ScaleY = -10,
			ScaleZ = -11,
			ScaleXY = -12,
			ScaleXZ = -13,
			ScaleZY = -14,
			ScaleXYZ = -15,
			RotateX = -16,
			RotateY = -17,
			RotateZ = -18,
			RotateXYZ = -19,
		};

		enum class ToolType
		{
			Grab,
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
		void SubmitLine(Vector2 a, Vector2 b, float width, Color color, int32_t id, LineMode mode);
		static void DrawQuad(EditorCamera camera, TransformComponent& transform, const Vector3& position, const Vector2& size, const Color& color, int
		                     data);
		static void DrawMeshVertices(EditorCamera camera, TransformComponent& transform, std::vector<MeshVertex>& vertices);

		static void DrawTool(Transform transform, EditorCamera camera, ToolType type, Tool operation = Tool::None);

		static void SetDisplayedVertices(Ref<std::vector<MeshVertex>> vertices);
		static void ClearDisplayedVertices();
	};
}
