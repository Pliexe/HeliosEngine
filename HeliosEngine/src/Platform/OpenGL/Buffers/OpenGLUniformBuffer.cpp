#include "OpenGLUniformBuffer.h"

namespace Helios
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t binding_slot, uint32_t size) : m_RendererID(0), m_BindingSlot(binding_slot), m_Size(size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_slot, m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t binding_slot, uint32_t size, void* data) : m_RendererID(0), m_BindingSlot(binding_slot), m_Size(size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_slot, m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, m_Size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
	}

	void OpenGLUniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}