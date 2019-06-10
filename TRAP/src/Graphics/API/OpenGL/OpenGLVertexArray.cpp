#include "TRAPPCH.h"
#include "OpenGLVertexArray.h"

TRAP::Graphics::API::OpenGLVertexArray::OpenGLVertexArray()
	: m_handle(0)
{
	TP_DEBUG("[VAO][OpenGL] Creating VertexArray");
	OpenGLCall(glCreateVertexArrays(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLVertexArray::~OpenGLVertexArray()
{
	TP_DEBUG("[VAO][OpenGL] Destroying VertexArray");
	OpenGLCall(glDeleteBuffers(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::AddBuffer(std::unique_ptr<VertexBuffer>& buffer)
{
	TP_DEBUG("[VAO][OpenGL] Adding VertexBuffer");
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
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Bind() const
{
	TP_DEBUG("[VAO][OpenGL] Binding VertexArray");
	OpenGLCall(glBindVertexArray(m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Unbind() const
{
	TP_DEBUG("[VAO][OpenGL] Unbinding VertexArray");
	OpenGLCall(glBindVertexArray(0));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLVertexArray::Draw(const uint32_t count) const
{
	OpenGLCall(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
}