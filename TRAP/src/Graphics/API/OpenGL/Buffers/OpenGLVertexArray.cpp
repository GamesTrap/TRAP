#include "TRAPPCH.h"
#include "OpenGLVertexArray.h"

TRAP::Graphics::API::OpenGLVertexArray::OpenGLVertexArray()
	: m_handle(0)
{
	OpenGLCall(glCreateVertexArrays(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexArray::~OpenGLVertexArray()
{
	OpenGLCall(glDeleteBuffers(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::AddVertexBuffer(std::unique_ptr<VertexBuffer>& buffer)
{
	TP_CORE_ASSERT(buffer->GetLayout().GetElements().size(), "[VBO][OpenGL] VertexBuffer has no layout!");

	Bind();
	buffer->Bind();

	uint32_t index = 0;
	const auto& layout = buffer->GetLayout();
	for (const auto& element : layout)
	{
		OpenGLCall(glEnableVertexAttribArray(index));
		OpenGLCall(glVertexAttribPointer(index,
			element.GetComponentCount(),
			TRAP::Graphics::API::ShaderDataTypeToOpenGLBaseType(element.Type),
			element.Normalized,
			layout.GetStride(),
			reinterpret_cast<const void*>(element.Offset))
		);

		index++;
	}

	m_vertexBuffers.push_back(std::move(buffer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::SetIndexBuffer(std::unique_ptr<IndexBuffer>& buffer)
{
	Bind();
	buffer->Bind();

	m_indexBuffer = std::move(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Graphics::API::VertexBuffer>>& TRAP::Graphics::API::OpenGLVertexArray::GetVertexBuffers()
{
	return m_vertexBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::IndexBuffer* TRAP::Graphics::API::OpenGLVertexArray::GetIndexBuffer()
{
	return m_indexBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Bind() const
{
	if(s_CurrentlyBound != this)
	{
		OpenGLCall(glBindVertexArray(m_handle));
		s_CurrentlyBound = this;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Unbind() const
{
	if(s_CurrentlyBound != nullptr)
	{
		OpenGLCall(glBindVertexArray(0));
		s_CurrentlyBound = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Draw() const
{
	OpenGLCall(glDrawElements(GL_TRIANGLES, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr));
}