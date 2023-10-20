#pragma once

#include "Helios/Resources/Buffer.h"

#ifdef HELIOS_PLATFORM_WINDOWS
namespace Helios
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:

		OpenGLIndexBuffer(uint32_t* indices, uint32_t count, BufferUsage usage);
		OpenGLIndexBuffer(uint32_t count, BufferUsage usage);
		~OpenGLIndexBuffer();

		void Bind() const override;
		void Unbind() const override;

		void SetData(const uint32_t* data, uint32_t count) override;
		std::string ToString() const override;

		uint32_t GetCount() const override;

	private:
		GLuint m_Buffer;
		uint32_t m_Count;
		uint32_t m_Size;
		uint32_t m_BindingSlot;
		uint32_t* m_Data;
	};
}
#endif