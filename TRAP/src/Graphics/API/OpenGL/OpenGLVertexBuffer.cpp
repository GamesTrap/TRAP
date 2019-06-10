#include "TRAPPCH.h"
#include "OpenGLVertexBuffer.h"

TRAP::Graphics::API::OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, const uint32_t size, const BufferUsage usage)
	: m_handle(0)
{
	TP_DEBUG("[VBO][OpenGL] Creating VertexBuffer");
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLCall(glBindBuffer(GL_ARRAY_BUFFER, m_handle));
	OpenGLCall(glBufferData(GL_ARRAY_BUFFER, size * sizeof(uint32_t), vertices, TRAPBufferUsageToOpenGL(usage)));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	TP_DEBUG("[VBO][OpenGL] Destroying VertexBuffer");
	OpenGLCall(glDeleteBuffers(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexBuffer::Bind()
{
	TP_DEBUG("[VBO][OpenGL] Binding VertexBuffer");
	OpenGLCall(glBindBuffer(GL_ARRAY_BUFFER, m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexBuffer::Unbind()
{
	TP_DEBUG("[VBO][OpenGL] Unbinding VertexBuffer");
	OpenGLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::API::BufferLayout& TRAP::Graphics::API::OpenGLVertexBuffer::GetLayout() const
{
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
{
	m_layout = layout;
}