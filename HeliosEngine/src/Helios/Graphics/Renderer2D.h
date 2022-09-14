#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Scene/Components.h"

namespace Helios {
	class HELIOS_API Renderer2D
	{
	private:

		

	public:
		static bool Init();
		static void Shutdown();

		static void BeginScene(Components::Transform& trans, Components::Camera& cam);
		static void EndScene();

		static void Flush();
		
		//static void DrawCube(Components::Transform transform, Components::SpriteRenderer sprite);
		static void DrawSprite(Components::Transform transform, Components::SpriteRenderer sprite);
		/*static void DrawTriangle(Vector2 position);
		static void DrawPolygon(int sides);*/
	};
}