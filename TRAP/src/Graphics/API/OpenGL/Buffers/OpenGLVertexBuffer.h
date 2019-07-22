#ifndef _TRAP_OPENGLVERTEXBUFFER_H_
#define _TRAP_OPENGLVERTEXBUFFER_H_

#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/BufferLayout.h"

namespace TRAP::Graphics::API
{
	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size, BufferUsage usage);
		OpenGLVertexBuffer(const OpenGLVertexBuffer&) = default;
		OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer&) = default;
		OpenGLVertexBuffer(OpenGLVertexBuffer&&) = default;
		OpenGLVertexBuffer& operator=(OpenGLVertexBuffer&&) = default;
		~OpenGLVertexBuffer();

		void Bind() override;
		void Unbind() override;

		const BufferLayout& GetLayout() const override;
		void SetLayout(const BufferLayout& layout) override;

	private:
		uint32_t m_handle;
		BufferLayout m_layout;
	};
}

#endif /*_TRAP_OPENGLVERTEXBUFFER_H_*/