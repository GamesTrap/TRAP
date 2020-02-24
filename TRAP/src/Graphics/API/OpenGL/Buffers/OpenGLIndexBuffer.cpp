#include "TRAPPCH.h"
#include "OpenGLIndexBuffer.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"

TRAP::Graphics::API::OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, const uint32_t size)
	: m_handle(0), m_count(size)
{
	TP_PROFILE_FUNCTION();
	
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLCall(glNamedBufferStorage(m_handle, size * sizeof(uint32_t), indices, GL_DYNAMIC_STORAGE_BIT));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	TP_PROFILE_FUNCTION();
	
	if(m_handle)
	{
		OpenGLCall(glDeleteBuffers(1, &m_handle));		
	}
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