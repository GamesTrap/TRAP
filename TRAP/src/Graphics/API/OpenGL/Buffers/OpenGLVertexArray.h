#ifndef _TRAP_OPENGLVERTEXARRAY_H_
#define _TRAP_OPENGLVERTEXARRAY_H_

namespace TRAP::Graphics::API
{
	class OpenGLVertexArray final : public VertexArray
	{
	public:
		OpenGLVertexArray();
		OpenGLVertexArray(const OpenGLVertexArray&) = default;
		OpenGLVertexArray& operator=(const OpenGLVertexArray&) = default;
		OpenGLVertexArray(OpenGLVertexArray&&) = default;
		OpenGLVertexArray& operator=(OpenGLVertexArray&&) = default;
		~OpenGLVertexArray();

		void AddVertexBuffer(std::unique_ptr<VertexBuffer>& buffer) override;
		void SetIndexBuffer(std::unique_ptr<IndexBuffer>& buffer) override;

		std::vector<std::unique_ptr<VertexBuffer>>& GetVertexBuffers() override;
		IndexBuffer* GetIndexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

	private:
		std::vector<std::unique_ptr<VertexBuffer>> m_vertexBuffers;
		std::unique_ptr<IndexBuffer> m_indexBuffer;

		uint32_t m_handle;
	};
}

#endif /*_TRAP_OPENGLVERTEXARRAY_H_*/