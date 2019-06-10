#include "TRAPPCH.h"
#include "OpenGLIndexBuffer.h"

TRAP::Graphics::API::OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, const uint32_t size, const BufferUsage usage)
	: m_handle(0), m_count(size)
{
	TP_DEBUG("[IBO][OpenGL] Creating IndexBuffer");
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle));
	OpenGLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), indices, TRAPBufferUsageToOpenGL(usage)));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	TP_DEBUG("[IBO][OpenGL] Destroying IndexBuffer");
	OpenGLCall(glDeleteBuffers(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLIndexBuffer::Bind() const
{
	TP_DEBUG("[IBO][OpenGL] Binding IndexBuffer");
	OpenGLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLIndexBuffer::Unbind() const
{
	TP_DEBUG("[IBO][OpenGL] Unbinding IndexBuffer");
	OpenGLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLIndexBuffer::GetCount() const
{
	return m_count;
}