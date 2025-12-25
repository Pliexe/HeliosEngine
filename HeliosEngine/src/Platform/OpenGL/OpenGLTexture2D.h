#pragma once
#include "Helios/Resources/Texture.h"

namespace Helios
{
	// class OpenGLTexture2D : public Texture2D
	// {
	// public:
	// 	OpenGLTexture2D(const std::filesystem::path& path);
	// 	OpenGLTexture2D(uint32_t width, uint32_t height);

	// 	~OpenGLTexture2D() override;

	// 	void Bind(uint32_t slot) override;
	// 	void Unbind() override;
	// 	void SetData(void* data, uint32_t size) override;
	// 	uint32_t GetWidth() const override;
	// 	uint32_t GetHeight() const override;
	// 	void* GetTextureID() override;
	// 	bool operator==(const Texture& other) const override;
	// 	bool operator!=(const Texture& other) const override;
	// private:
	// 	uint32_t m_RendererID;
	// 	uint32_t m_Width, m_Height;
	// 	uint32_t m_lastBoundSlot = 0u;
	// 	uint32_t m_dataFormat;
	// };
}
