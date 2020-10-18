#ifndef _TRAP_OPENGLVERTEXBUFFER_H_
#define _TRAP_OPENGLVERTEXBUFFER_H_

#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/BufferLayout.h"

namespace TRAP::Graphics::API
{
	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		explicit OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const OpenGLVertexBuffer&) = default;
		OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer&) = default;
		OpenGLVertexBuffer(OpenGLVertexBuffer&&) = default;
		OpenGLVertexBuffer& operator=(OpenGLVertexBuffer&&) = default;
		~OpenGLVertexBuffer();

		const BufferLayout& GetLayout() const override;
		void SetLayout(const BufferLayout& layout) override;
		uint32_t GetVertexCount() const override;
		void SetData(const void* data, uint32_t size) override;
		
		uint32_t GetHandle() const;

	private:
		void CreateBuffer(float* vertices, uint32_t size);
		
		uint32_t m_handle;
		BufferLayout m_layout;
		uint32_t m_vertexCount;
	};
}

#endif /*_TRAP_OPENGLVERTEXBUFFER_H_*/