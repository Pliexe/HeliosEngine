#include "OpenGLIndexBuffer.h"
#include "OpenGLUsage.h"

namespace Helios
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count, BufferUsage usage)
	{
		glGenBuffers(1, &m_Buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), (void*)indices, GetBufferUsage(usage));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_Count = count;
		m_Data = new uint32_t[count];
		std::memcpy(m_Data, indices, count * sizeof(uint32_t));
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, BufferUsage usage)
	{
		glGenBuffers(1, &m_Buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, GetBufferUsage(usage));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		m_Count = count;
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_Buffer);
		delete[] m_Data;
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(const uint32_t* data, uint32_t count)
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(uint32_t), data);
	}

	uint32_t OpenGLIndexBuffer::GetCount() const
	{
		return m_Count;
	}

	std::string OpenGLIndexBuffer::ToString() const
	{
		std::string str = std::to_string(m_Data[0]);

        for (uint32_t i = 1; i < m_Count; i++)
        {
         	str += (i % 3 == 0 ? "\n" : ", ") + std::to_string(m_Data[i]);
        }

        return str;
	}

}