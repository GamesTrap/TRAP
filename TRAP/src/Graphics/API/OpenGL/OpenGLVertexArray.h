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

		void AddBuffer(std::unique_ptr<VertexBuffer>& buffer) override;

		void Bind() const override;
		void Unbind() const override;

		void Draw(uint32_t count) const override;

	private:
		uint32_t m_handle;
	};
}

#endif /*_TRAP_OPENGLVERTEXARRAY_H_*/