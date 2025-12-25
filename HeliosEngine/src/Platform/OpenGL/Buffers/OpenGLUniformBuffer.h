#pragma once

#include "pch.h"
#include "Helios/Resources/Buffer.h"

namespace Helios
{
	class OpenGLUniformBuffer : public UnsafeUniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t binding_slot, uint32_t size);
		OpenGLUniformBuffer(uint32_t binding_slot, uint32_t size, void* data);
		~OpenGLUniformBuffer();

		void SetData(const void* data) override;

		// void Bind() const override;
		// void Unbind() const override;

	private:
		uint32_t m_RendererID;
		uint32_t m_BindingSlot;
		uint32_t m_Size;
	};
}