#include "TRAPPCH.h"
#include "OpenGLVertexArray.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/BufferLayout.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"

TRAP::Graphics::API::OpenGLVertexArray::OpenGLVertexArray()
	: m_indexCount(0), m_attribIndex(0), m_bindingIndex(0), m_handle(0)
{
	TP_PROFILE_FUNCTION();
	
	OpenGLCall(glCreateVertexArrays(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexArray::~OpenGLVertexArray()
{
	TP_PROFILE_FUNCTION();
	
	if(m_handle)
	{
		OpenGLCall(glDeleteBuffers(1, &m_handle));		
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::AddVertexBuffer(Scope<VertexBuffer>& buffer)
{
	TRAP_CORE_ASSERT(buffer->GetLayout().GetElements().size(), "[VBO][OpenGL] VertexBuffer has no layout!");
	
	TP_PROFILE_FUNCTION();
	
	OpenGLCall(glVertexArrayVertexBuffer(m_handle, m_bindingIndex, dynamic_cast<OpenGLVertexBuffer*>(buffer.get())->GetHandle(), 0, buffer->GetLayout().GetStride()));

	uint32_t components = 0;
	const auto& layout = buffer->GetLayout();
	for (const auto& element : layout)
	{
		OpenGLCall(glEnableVertexArrayAttrib(m_handle, m_attribIndex));
		OpenGLCall(glVertexArrayAttribFormat(m_handle, m_attribIndex, element.GetComponentCount(), TRAP::Graphics::API::ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized, static_cast<intptr_t>(element.Offset)));
		OpenGLCall(glVertexArrayAttribBinding(m_handle, m_attribIndex, m_bindingIndex));
		components += element.GetComponentCount();
		
		m_attribIndex++;
	}

	m_indexCount += buffer->GetVertexCount() / components;

	m_vertexBuffers.emplace_back(std::move(buffer));

	m_bindingIndex++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::SetIndexBuffer(Scope<IndexBuffer>& buffer)
{
	TP_PROFILE_FUNCTION();
	
	m_indexBuffer = std::move(buffer);
	OpenGLCall(glVertexArrayElementBuffer(m_handle, dynamic_cast<OpenGLIndexBuffer*>(m_indexBuffer.get())->GetHandle()));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Bind() const
{
	TP_PROFILE_FUNCTION();
	
	if(s_CurrentlyBound != this)
	{
		OpenGLCall(glBindVertexArray(m_handle));
		s_CurrentlyBound = this;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Unbind() const
{
	TP_PROFILE_FUNCTION();
	
	OpenGLCall(glBindVertexArray(0));
	s_CurrentlyBound = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Scope<TRAP::Graphics::VertexBuffer>>& TRAP::Graphics::API::OpenGLVertexArray::GetVertexBuffers()
{
	TP_PROFILE_FUNCTION();
	
	return m_vertexBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::IndexBuffer>& TRAP::Graphics::API::OpenGLVertexArray::GetIndexBuffer()
{
	TP_PROFILE_FUNCTION();
	
	return m_indexBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLVertexArray::GetIndexCount() const
{
	TP_PROFILE_FUNCTION();
	
	return m_indexCount;
}