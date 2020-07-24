#include "TRAPPCH.h"
#include "OpenGLUniformBuffer.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/API/OpenGL/OpenGLShader.h"

uint32_t TRAP::Graphics::API::OpenGLUniformBuffer::s_maxUniformBufferBindingPoints = 0;

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<uint32_t, const TRAP::Graphics::API::OpenGLUniformBuffer*> TRAP::Graphics::API::OpenGLUniformBuffer::s_boundUniformBuffers{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::OpenGLUniformBuffer(std::string name, const void* data, const uint32_t size, const BufferUsage usage)
	: m_handle(0), m_name(std::move(name)), m_size(size), m_usage(usage)
{
	TP_PROFILE_FUNCTION();
	
	if(s_maxUniformBufferBindingPoints == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, reinterpret_cast<int32_t*>(&s_maxUniformBufferBindingPoints)));
	}
	
	OpenGLCall(glCreateBuffers(1, &m_handle));
	if (m_usage == BufferUsage::Static)
	{
		OpenGLCall(glNamedBufferStorage(m_handle, m_size, data, 0));
	}
	else if (m_usage == BufferUsage::Dynamic)
	{
		OpenGLCall(glNamedBufferStorage(m_handle, m_size, data, GL_DYNAMIC_STORAGE_BIT));
	}
	else //Stream
	{
		OpenGLCall(glNamedBufferStorage(m_handle, m_size, data, GL_DYNAMIC_STORAGE_BIT));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::OpenGLUniformBuffer(std::string name, const uint32_t size, const BufferUsage usage)
	: m_handle(0), m_name(std::move(name)), m_size(size), m_usage(usage)
{
	TP_PROFILE_FUNCTION();
	
	if (s_maxUniformBufferBindingPoints == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, reinterpret_cast<int32_t*>(&s_maxUniformBufferBindingPoints)));
	}
	
	OpenGLCall(glCreateBuffers(1, &m_handle));
	if (m_usage == BufferUsage::Static)
	{
		OpenGLCall(glNamedBufferStorage(m_handle, m_size, nullptr, 0));
	}
	else if (m_usage == BufferUsage::Dynamic)
	{
		OpenGLCall(glNamedBufferStorage(m_handle, m_size, nullptr, GL_DYNAMIC_STORAGE_BIT));
	}
	else //Stream
	{
		OpenGLCall(glNamedBufferStorage(m_handle, m_size, nullptr, GL_DYNAMIC_STORAGE_BIT));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
	TP_PROFILE_FUNCTION();
	
	if (m_handle)
	{
		OpenGLCall(glDeleteBuffers(1, &m_handle));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::Bind(const uint32_t bindingPoint) const
{
	TP_PROFILE_FUNCTION();
	
	if (bindingPoint < s_maxUniformBufferBindingPoints)
	{
		if (s_boundUniformBuffers[bindingPoint] != this)
		{
			for (const auto& [name, shader] : ShaderManager::GetShaders())
			{
				uint32_t uniformBlockIndex = GL_INVALID_INDEX;
				if (dynamic_cast<OpenGLShader*>(shader.get())->GetHandle())
				{
					OpenGLCall(uniformBlockIndex = glGetUniformBlockIndex(dynamic_cast<OpenGLShader*>(shader.get())->GetHandle(), m_name.c_str()));
				}

				if (uniformBlockIndex != GL_INVALID_INDEX)
				{
					OpenGLCall(glUniformBlockBinding(dynamic_cast<OpenGLShader*>(shader.get())->GetHandle(), uniformBlockIndex, bindingPoint));
				}
			}

			OpenGLCall(glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_handle));
			s_boundUniformBuffers[bindingPoint] = this;
		}
	}
	else
		TP_ERROR(Log::UniformBufferOpenGLPrefix, "Couldn't bind UniformBuffer: \"", m_name, "\" to binding point: ", bindingPoint, "! Out of range");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::Unbind(const uint32_t bindingPoint) const
{
	TP_PROFILE_FUNCTION();
	
	if(bindingPoint < s_maxUniformBufferBindingPoints)
	{
		OpenGLCall(glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, 0));
		s_boundUniformBuffers[bindingPoint] = nullptr;
	}
	else
		TP_ERROR(Log::UniformBufferOpenGLPrefix, "Couldn't unbind UniformBuffer: \"", m_name, "\" from binding point: ", bindingPoint, "! Out of range");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::UpdateData(const void* data)
{
	TP_PROFILE_FUNCTION();
	
	if (m_usage != BufferUsage::Static)
	{
		if (m_usage == BufferUsage::Dynamic)
		{
			OpenGLCall(glNamedBufferSubData(m_handle, 0, m_size, data));
		}
		else //Stream
		{
			OpenGLCall(glNamedBufferSubData(m_handle, 0, m_size, data));
		}
	}
	else
		TP_ERROR(Log::UniformBufferOpenGLPrefix, "Static UniformBuffer: \"", m_name, "\" tried to update data!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::UpdateSubData(const void* data, const uint32_t size, const uint32_t offset)
{
	TP_PROFILE_FUNCTION();
	
	if (m_usage == BufferUsage::Dynamic)
	{
		if (size == 0)
		{
			TP_ERROR(Log::UniformBufferOpenGLPrefix, "UniformBuffer: \"", m_name, "\" update Size: ", size, " cant be 0!");
			return;
		}
		if (offset + size > m_size)
		{
			TP_ERROR(Log::UniformBufferOpenGLPrefix, "Dynamic UniformBuffer: \"", m_name, "\" update Size: ", size, " with Offset: ", offset, " is bigger than the current UniformBuffer Size: ", m_size, "!");
			return;
		}
		
		OpenGLCall(glNamedBufferSubData(m_handle, offset, size, data));
	}
	else
	{
		if(m_usage == BufferUsage::Static)
			TP_ERROR(Log::UniformBufferOpenGLPrefix, "Static UniformBuffer: \"", m_name, "\" tried to update sub data!");
		else
			TP_ERROR(Log::UniformBufferOpenGLPrefix, "Stream UniformBuffer: \"", m_name, "\" tried to update sub data!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::OpenGLUniformBuffer::GetName() const
{
	TP_PROFILE_FUNCTION();
	
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLUniformBuffer::GetSize() const
{
	TP_PROFILE_FUNCTION();
	
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferUsage TRAP::Graphics::API::OpenGLUniformBuffer::GetUsage() const
{
	TP_PROFILE_FUNCTION();
	
	return m_usage;
}