#include "TRAPPCH.h"
#include "OpenGLUniformBuffer.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/API/OpenGL/OpenGLShader.h"

TRAP::Graphics::API::OpenGLUniformBuffer::OpenGLUniformBuffer()
	: m_handle(0), m_name(""), m_size(0)
{	
	OpenGLCall(glCreateBuffers(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::OpenGLUniformBuffer(const char* name, const void* data, const uint32_t size)
	: m_handle(0), m_name(name), m_size(size)
{
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLCall(glNamedBufferStorage(m_handle, m_size, data, GL_DYNAMIC_STORAGE_BIT));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::OpenGLUniformBuffer(const char* name, const uint32_t size)
	: m_handle(0), m_name(name), m_size(size)
{
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLCall(glNamedBufferStorage(m_handle, m_size, nullptr, GL_DYNAMIC_STORAGE_BIT));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
	if(m_handle)
	{
		OpenGLCall(glDeleteBuffers(1, &m_handle));		
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::Bind() const
{
	OpenGLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_handle));

	for (const auto& shader : ShaderManager::GetShaders())
	{
		uint32_t uniformBlockIndex = GL_INVALID_INDEX;
		if(dynamic_cast<OpenGLShader*>(shader.second.get())->GetHandle())
		{			
			OpenGLCall(uniformBlockIndex = glGetUniformBlockIndex(dynamic_cast<OpenGLShader*>(shader.second.get())->GetHandle(), m_name));
		}

		if (uniformBlockIndex != GL_INVALID_INDEX)
		{			
			OpenGLCall(glUniformBlockBinding(dynamic_cast<OpenGLShader*>(shader.second.get())->GetHandle(), uniformBlockIndex, 0));		
		}
	}

	OpenGLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::Unbind() const
{
	OpenGLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::UpdateData(const void* data, const uint32_t size)
{
	OpenGLCall(glNamedBufferSubData(m_handle, 0, size, data));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::UpdateSubData(const void* data, const uint32_t size, const uint32_t offset)
{
	OpenGLCall(glNamedBufferSubData(m_handle, offset, size, data));
}