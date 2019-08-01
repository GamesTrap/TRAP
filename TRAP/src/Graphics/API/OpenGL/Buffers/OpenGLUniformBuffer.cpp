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
	OpenGLUniformBuffer::Bind();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::OpenGLUniformBuffer(const char* name, const void* data, const uint32_t size, const BufferUsage usage)
	: m_handle(0), m_name(name), m_size(size)
{
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLUniformBuffer::Bind();
	OpenGLCall(glBufferData(GL_UNIFORM_BUFFER, m_size, data, TRAPBufferUsageToOpenGL(usage)));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::OpenGLUniformBuffer(const char* name, const uint32_t size, const BufferUsage usage)
	: m_handle(0), m_name(name), m_size(size)
{
	OpenGLCall(glCreateBuffers(1, &m_handle));
	OpenGLUniformBuffer::Bind();
	OpenGLCall(glBufferData(GL_UNIFORM_BUFFER, m_size, nullptr, TRAPBufferUsageToOpenGL(usage)));
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

	for (const std::unique_ptr<Graphics::Shader>& shader : ShaderManager::GetShaders())
	{
		uint32_t uniformBlockIndex = GL_INVALID_INDEX;
		if(dynamic_cast<OpenGLShader*>(shader.get())->GetHandle())
		{			
			OpenGLCall(uniformBlockIndex = glGetUniformBlockIndex(dynamic_cast<OpenGLShader*>(shader.get())->GetHandle(), m_name));
		}

		if (uniformBlockIndex != GL_INVALID_INDEX)
		{			
			OpenGLCall(glUniformBlockBinding(dynamic_cast<OpenGLShader*>(shader.get())->GetHandle(), uniformBlockIndex, 0));		
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

void TRAP::Graphics::API::OpenGLUniformBuffer::UpdateData(const void* data, const uint32_t size, const BufferUsage usage)
{
	OpenGLCall(glBufferData(GL_UNIFORM_BUFFER, size, data, TRAPBufferUsageToOpenGL(usage)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::UpdateSubData(const void* data, const uint32_t size, const uint32_t offset)
{
	OpenGLCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
}