#include "TRAPPCH.h"
#include "OpenGLVertexArray.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/BufferLayout.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"

TRAP::Graphics::API::OpenGLVertexArray::OpenGLVertexArray()
	: m_attribIndex(0), m_handle(0)
{
	TP_PROFILE_FUNCTION();
	
	glCreateVertexArrays(1, &m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexArray::~OpenGLVertexArray()
{
	TP_PROFILE_FUNCTION();
	
	if(m_handle)
	{
		glDeleteBuffers(1, &m_handle);		
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::SetVertexBuffer(Scope<VertexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();
	
	TRAP_CORE_ASSERT(buffer->GetLayout().GetElements().size(), "[VBO][OpenGL] VertexBuffer has no layout!");
	
	glVertexArrayVertexBuffer(m_handle, 0, dynamic_cast<OpenGLVertexBuffer*>(buffer.get())->GetHandle(), 0, buffer->GetLayout().GetStride());

	const auto& layout = buffer->GetLayout();
	for (const auto& element : layout)
	{
		switch(element.Type)
		{
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
		case ShaderDataType::Bool:
		{
			glEnableVertexArrayAttrib(m_handle, m_attribIndex);
			glVertexArrayAttribFormat(m_handle, m_attribIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized, static_cast<intptr_t>(element.Offset));
			glVertexArrayAttribBinding(m_handle, m_attribIndex, 0);

			m_attribIndex++;
			break;
		}

		case ShaderDataType::Mat3:
		case ShaderDataType::Mat4:
		{
			const uint8_t count = element.GetComponentCount();
			for (uint8_t i = 0; i < count; i++)
			{
				glEnableVertexArrayAttrib(m_handle, m_attribIndex);
				glVertexArrayAttribFormat(m_handle, m_attribIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized, static_cast<intptr_t>(sizeof(float) * count * i));
				glVertexArrayAttribBinding(m_handle, m_attribIndex, 0);
				glVertexArrayBindingDivisor(m_handle, 0, 1);

				m_attribIndex++;
			}			
			break;
		}

		default:
			TRAP_CORE_ASSERT(false, "[VAO][OpenGL] Unknown ShaderDataType!");
		}
	}

	m_vertexBuffer = std::move(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::SetIndexBuffer(Scope<IndexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();
	
	m_indexBuffer = std::move(buffer);
	glVertexArrayElementBuffer(m_handle, dynamic_cast<OpenGLIndexBuffer*>(m_indexBuffer.get())->GetHandle());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Bind() const
{
	TP_PROFILE_FUNCTION();
	
	if(s_CurrentlyBound != this)
	{
		glBindVertexArray(m_handle);
		s_CurrentlyBound = this;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Unbind() const
{
	TP_PROFILE_FUNCTION();
	
	glBindVertexArray(0);
	s_CurrentlyBound = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::VertexBuffer>& TRAP::Graphics::API::OpenGLVertexArray::GetVertexBuffer()
{
	TP_PROFILE_FUNCTION();
	
	return m_vertexBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::IndexBuffer>& TRAP::Graphics::API::OpenGLVertexArray::GetIndexBuffer()
{
	TP_PROFILE_FUNCTION();
	
	return m_indexBuffer;
}