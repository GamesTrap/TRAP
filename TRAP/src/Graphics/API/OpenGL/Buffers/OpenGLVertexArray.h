#ifndef _TRAP_OPENGLVERTEXARRAY_H_
#define _TRAP_OPENGLVERTEXARRAY_H_

#include "Graphics/Buffers/VertexArray.h"

namespace TRAP::Graphics::API
{
	class OpenGLVertexArray final : public VertexArray
	{
	public:
		OpenGLVertexArray();
		OpenGLVertexArray(const OpenGLVertexArray&) = delete;
		OpenGLVertexArray& operator=(const OpenGLVertexArray&) = delete;
		OpenGLVertexArray(OpenGLVertexArray&&) = default;
		OpenGLVertexArray& operator=(OpenGLVertexArray&&) = delete;
		~OpenGLVertexArray();

		void AddVertexBuffer(Scope<VertexBuffer>& buffer) override;
		void SetIndexBuffer(Scope<IndexBuffer>& buffer) override;

		void Bind() const override;
		void Unbind() const override;
		
		std::vector<Scope<VertexBuffer>>& GetVertexBuffers() override;
		const Scope<IndexBuffer>& GetIndexBuffer() override;

	private:
		std::vector<Scope<VertexBuffer>> m_vertexBuffers;
		Scope<IndexBuffer> m_indexBuffer;
		uint32_t m_attribIndex;
		uint32_t m_bindingIndex;

		uint32_t m_handle;
	};
}

#endif /*_TRAP_OPENGLVERTEXARRAY_H_*/