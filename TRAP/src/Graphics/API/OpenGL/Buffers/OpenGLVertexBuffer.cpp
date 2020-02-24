#include "TRAPPCH.h"
#include "OpenGLVertexBuffer.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"

TRAP::Graphics::API::OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, const uint32_t size)
	: m_handle(0), m_vertexCount(size)
{
	TP_PROFILE_FUNCTION();
	
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLCall(glNamedBufferStorage(m_handle, size * sizeof(uint32_t), vertices, GL_DYNAMIC_STORAGE_BIT));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	TP_PROFILE_FUNCTION();
	
	if(m_handle)
	{
		OpenGLCall(glDeleteBuffers(1, &m_handle));		
	}
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

uint32_t TRAP::Graphics::API::OpenGLVertexBuffer::GetHandle() const
{
	TP_PROFILE_FUNCTION();
	
	return m_handle;
}