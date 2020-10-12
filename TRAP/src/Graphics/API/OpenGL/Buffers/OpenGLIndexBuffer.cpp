#include "TRAPPCH.h"
#include "OpenGLIndexBuffer.h"

TRAP::Graphics::API::OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, const uint32_t size)
	: m_handle(0), m_count(size)
{
	TP_PROFILE_FUNCTION();
	
	glCreateBuffers(1, &m_handle);
	glNamedBufferStorage(m_handle, size * sizeof(uint32_t), indices, GL_DYNAMIC_STORAGE_BIT);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	TP_PROFILE_FUNCTION();
	
	if(m_handle)
		glDeleteBuffers(1, &m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLIndexBuffer::GetCount() const
{
	TP_PROFILE_FUNCTION();
	
	return m_count;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLIndexBuffer::GetHandle() const
{
	TP_PROFILE_FUNCTION();
	
	return m_handle;
}