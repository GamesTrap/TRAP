#include "TRAPPCH.h"
#include "OpenGLVertexBuffer.h"

TRAP::Graphics::API::OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, const uint32_t size)
	: m_handle(0), m_vertexCount(size)
{
	TP_PROFILE_FUNCTION();

	CreateBuffer(vertices, size);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexBuffer::OpenGLVertexBuffer(const uint32_t size)
	: m_handle(0), m_vertexCount(size)
{
	TP_PROFILE_FUNCTION();

	CreateBuffer(nullptr, size);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	TP_PROFILE_FUNCTION();
	
	if(m_handle)
		glDeleteBuffers(1, &m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::BufferLayout& TRAP::Graphics::API::OpenGLVertexBuffer::GetLayout() const
{
	TP_PROFILE_FUNCTION();
	
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
{
	TP_PROFILE_FUNCTION();
	
	m_layout = layout;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLVertexBuffer::GetVertexCount() const
{
	TP_PROFILE_FUNCTION();
	
	return m_vertexCount;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexBuffer::SetData(const void* data, const uint32_t size)
{
	TP_PROFILE_FUNCTION();

	glNamedBufferSubData(m_handle, 0, size, data);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLVertexBuffer::GetHandle() const
{
	TP_PROFILE_FUNCTION();
	
	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexBuffer::CreateBuffer(float* vertices, const uint32_t size)
{
	glCreateBuffers(1, &m_handle);
	glNamedBufferStorage(m_handle, size, vertices, GL_DYNAMIC_STORAGE_BIT);
}