#pragma once

#include "pch.h"
#include "Helios/Resources/Buffer.h"

namespace Helios
{
	GLenum GetBufferUsage(BufferUsage usage)
	{
		switch (usage)
		{
		case BufferUsage::Static:
			return GL_STATIC_DRAW;
		case BufferUsage::Dynamic:
			return GL_DYNAMIC_DRAW;
		case BufferUsage::Stream:
			return GL_STREAM_DRAW;
		case BufferUsage::Staging:
			return GL_READ_WRITE;
		}
	}
}