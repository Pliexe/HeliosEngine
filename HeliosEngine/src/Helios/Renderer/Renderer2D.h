#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Scene/Components.h"

namespace Helios {
	class HELIOS_API Renderer2D
	{
	private:


		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> s_renderTarget;
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> s_shaderResourceView;
		static Microsoft::WRL::ComPtr<ID3D11Texture2D> s_renderTargetTexture;

		static unsigned int width;
		static unsigned int height;

		static bool CreateRenderTarget(UINT width, UINT height);

	public:
		static bool Init();
		static void Shutdown();

		static void BeginScene(Components::Transform& trans, Components::Camera& cam);
		static void EndScene();

		static void Resize(unsigned int width, unsigned int height);

		static inline ImTextureID GetImGuiTexture();
		static ImVec2 GetRenderTargetSize();
		
		static void DrawSprite(Components::Transform transform, Components::SpriteRenderer sprite);
		static void DrawTriangle(Vector2 position);
		static void DrawPolygon(int sides);
	};
}