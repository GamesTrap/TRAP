#include "TRAPPCH.h"
#include "OpenGLUniformBuffer.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/API/OpenGL/OpenGLShader.h"

uint32_t TRAP::Graphics::API::OpenGLUniformBuffer::s_uniformBufferOffsetAlignment = 0;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::OpenGLUniformBuffer(const char* name, const void* data, const uint32_t size, const BufferUsage usage)
	: m_handle(0), m_name(name), m_size(size), m_usage(usage), m_dataPtr(nullptr), m_nextHeadOffset(0), m_bufferCount(0), m_alignedSize(size)
{
	if(s_uniformBufferOffsetAlignment == 0)
	{
		OpenGLCall(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, reinterpret_cast<int32_t*>(&s_uniformBufferOffsetAlignment)));
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
		while (m_alignedSize % s_uniformBufferOffsetAlignment != 0)
			++m_alignedSize;
		OpenGLCall(glNamedBufferStorage(m_handle, m_alignedSize * 3, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
		OpenGLCall(m_dataPtr = static_cast<uint8_t*>(glMapNamedBufferRange(m_handle, 0, m_alignedSize * 3, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)));

		UpdateData(data);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::OpenGLUniformBuffer(const char* name, const uint32_t size, const BufferUsage usage)
	: m_handle(0), m_name(name), m_size(size), m_usage(usage), m_dataPtr(nullptr), m_nextHeadOffset(0), m_bufferCount(0), m_alignedSize(size)
{
	if (s_uniformBufferOffsetAlignment == 0)
	{
		OpenGLCall(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, reinterpret_cast<int32_t*>(&s_uniformBufferOffsetAlignment)));
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
		while (m_alignedSize % s_uniformBufferOffsetAlignment != 0)
			++m_alignedSize;
		OpenGLCall(glNamedBufferStorage(m_handle, m_alignedSize * 3, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
		OpenGLCall(m_dataPtr = static_cast<uint8_t*>(glMapNamedBufferRange(m_handle, 0, m_alignedSize * 3, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
	if (m_handle)
	{
		if (m_dataPtr && m_usage == BufferUsage::Stream)
		{			
			OpenGLCall(glUnmapNamedBuffer(m_handle));
		}

		OpenGLCall(glDeleteBuffers(1, &m_handle));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::Bind(const uint32_t bindingPoint) const
{
	for (const auto& [name, shader] : ShaderManager::GetShaders())
	{
		uint32_t uniformBlockIndex = GL_INVALID_INDEX;
		if (dynamic_cast<OpenGLShader*>(shader.get())->GetHandle())
		{
			OpenGLCall(uniformBlockIndex = glGetUniformBlockIndex(dynamic_cast<OpenGLShader*>(shader.get())->GetHandle(), m_name));
		}

		if (uniformBlockIndex != GL_INVALID_INDEX)
		{
			OpenGLCall(glUniformBlockBinding(dynamic_cast<OpenGLShader*>(shader.get())->GetHandle(), uniformBlockIndex, bindingPoint));
		}
	}

	OpenGLCall(glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, m_handle, m_bufferCount * m_alignedSize, m_size));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::Unbind() const
{
	OpenGLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::UpdateData(const void* data)
{	
	if (m_usage != BufferUsage::Static)
	{
		if (m_usage == BufferUsage::Dynamic)
		{
			OpenGLCall(glNamedBufferSubData(m_handle, 0, m_size, data));
		}
		else //Stream
		{
			//Need to wait for this area to become available.
			//If everything is sized properly, it will always be available right away(no stalling) :D

			std::memcpy(m_dataPtr + m_nextHeadOffset, data, m_size);

			//Lock
			
			if (m_nextHeadOffset == 0)
				m_bufferCount = 0;
			else if (m_nextHeadOffset == m_alignedSize)
				m_bufferCount = 1;
			else
				m_bufferCount = 2;
			m_nextHeadOffset = (m_nextHeadOffset + m_alignedSize) % (m_alignedSize * 3);
		}
	}
	else
		TP_ERROR("[UniformBuffer][OpenGL] Static UniformBuffer: \"", m_name, "\" tried to update data!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLUniformBuffer::UpdateSubData(const void* data, const uint32_t size, const uint32_t offset)
{
	if (m_usage == BufferUsage::Dynamic)
	{
		if (size == 0)
		{
			TP_ERROR("[UniformBuffer][OpenGL] UniformBuffer: \"", m_name, "\" update Size: ", size, " cant be 0!");
			return;
		}
		if (offset + size > m_size)
		{
			TP_ERROR("[UniformBuffer][OpenGL] Dynamic UniformBuffer: \"", m_name, "\" update Size: ", size, " with Offset: ", offset, " is bigger than the current UniformBuffer Size: ", m_size, "!");
			return;
		}
		
		OpenGLCall(glNamedBufferSubData(m_handle, offset, size, data));
	}
	else
	{
		if(m_usage == BufferUsage::Static)
			TP_ERROR("[UniformBuffer][OpenGL] Static UniformBuffer: \"", m_name, "\" tried to update sub data!");
		else
			TP_ERROR("[UniformBuffer][OpenGL] Stream UniformBuffer: \"", m_name, "\" tried to update sub data!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::OpenGLUniformBuffer::GetName() const
{
	return std::string(m_name);
}