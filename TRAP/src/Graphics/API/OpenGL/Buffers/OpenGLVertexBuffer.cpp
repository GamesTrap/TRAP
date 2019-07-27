#include "TRAPPCH.h"
#include "OpenGLVertexBuffer.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"

TRAP::Graphics::API::OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, const uint32_t size, const BufferUsage usage)
	: m_handle(0)
{
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLCall(glBindBuffer(GL_ARRAY_BUFFER, m_handle));
	OpenGLCall(glBufferData(GL_ARRAY_BUFFER, size * sizeof(uint32_t), vertices, TRAPBufferUsageToOpenGL(usage)));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	OpenGLCall(glDeleteBuffers(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexBuffer::Bind()
{
	OpenGLCall(glBindBuffer(GL_ARRAY_BUFFER, m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexBuffer::Unbind()
{
	OpenGLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
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