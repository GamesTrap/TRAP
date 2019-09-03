#include "TRAPPCH.h"
#include "OpenGLVertexBuffer.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"

TRAP::Graphics::API::OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, const uint32_t size)
	: m_handle(0), m_vertexCount(size)
{
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLCall(glNamedBufferStorage(m_handle, size * sizeof(uint32_t), vertices, GL_DYNAMIC_STORAGE_BIT));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	if(m_handle)
	{
		OpenGLCall(glDeleteBuffers(1, &m_handle));		
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::BufferLayout& TRAP::Graphics::API::OpenGLVertexBuffer::GetLayout() const
{
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
{
	m_layout = layout;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLVertexBuffer::GetHandle() const
{
	return m_handle;
}