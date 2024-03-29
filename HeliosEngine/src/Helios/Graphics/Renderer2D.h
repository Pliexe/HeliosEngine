#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Scene/Components.h"
#include "Helios/Math/Matrix.h"

namespace Helios {
	class HELIOS_API Renderer2D
	{
	private:

		

	public:
		static bool Init();
		static void Shutdown();

		static void BeginScene(Matrix4x4 projection);
		static void EndScene();

		static void Flush();
		
		//static void DrawCube(TransformComponent transform, SpriteRendererComponent sprite);
		static void DrawSprite(uint32_t entityId, Matrix4x4 worldMatrix, SpriteRendererComponent sprite);
		/*static void DrawTriangle(Vector2 position);
		static void DrawPolygon(int sides);*/
	};
}