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
		OpenGLVertexArray& operator=(OpenGLVertexArray&&) = default;
		~OpenGLVertexArray();

		void AddVertexBuffer(std::unique_ptr<VertexBuffer>& buffer) override;
		void SetIndexBuffer(std::unique_ptr<IndexBuffer>& buffer) override;

		void Bind() const override;
		void Unbind() const override;
		
		std::vector<std::unique_ptr<VertexBuffer>>& GetVertexBuffers() override;
		IndexBuffer* GetIndexBuffer() override;

	private:
		std::vector<std::unique_ptr<VertexBuffer>> m_vertexBuffers;
		std::unique_ptr<IndexBuffer> m_indexBuffer;

		uint32_t m_handle;
	};
}

#endif /*_TRAP_OPENGLVERTEXARRAY_H_*/