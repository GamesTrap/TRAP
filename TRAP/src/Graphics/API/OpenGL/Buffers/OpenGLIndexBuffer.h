#ifndef _TRAP_OPENGLINDEXBUFFER_H_
#define _TRAP_OPENGLINDEXBUFFER_H_

#include "Graphics/Buffers/IndexBuffer.h"

namespace TRAP::Graphics::API
{
	class OpenGLIndexBuffer final : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size, BufferUsage usage);
		OpenGLIndexBuffer(const OpenGLIndexBuffer&) = default;
		OpenGLIndexBuffer& operator=(const OpenGLIndexBuffer&) = default;
		OpenGLIndexBuffer(OpenGLIndexBuffer&&) = default;
		OpenGLIndexBuffer& operator=(OpenGLIndexBuffer&&) = default;
		virtual ~OpenGLIndexBuffer();

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const override;

	private:
		uint32_t m_handle;
		uint32_t m_count;
	};
}

#endif /*_TRAP_OPENGLINDEXBUFFER_H_*/