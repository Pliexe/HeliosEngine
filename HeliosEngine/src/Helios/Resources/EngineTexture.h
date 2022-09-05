#pragma once

#include "Helios/Core.h"
#include "pch.h"

namespace Helios {
	class HELIOS_API EngineTexture {
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

		int image_width;
		int image_height;

	public:

		EngineTexture() = delete;
		EngineTexture(const char* filename);

		operator void* ();
	};
}