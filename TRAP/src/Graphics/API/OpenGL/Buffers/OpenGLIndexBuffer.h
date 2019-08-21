#ifndef _TRAP_OPENGLINDEXBUFFER_H_
#define _TRAP_OPENGLINDEXBUFFER_H_

#include "Graphics/Buffers/IndexBuffer.h"

namespace TRAP::Graphics::API
{
	class OpenGLIndexBuffer final : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		OpenGLIndexBuffer(const OpenGLIndexBuffer&) = default;
		OpenGLIndexBuffer& operator=(const OpenGLIndexBuffer&) = default;
		OpenGLIndexBuffer(OpenGLIndexBuffer&&) = default;
		OpenGLIndexBuffer& operator=(OpenGLIndexBuffer&&) = default;
		virtual ~OpenGLIndexBuffer();

		uint32_t GetCount() const override;

		uint32_t GetHandle() const;

	private:
		uint32_t m_handle;
		uint32_t m_count;
	};
}

#endif /*_TRAP_OPENGLINDEXBUFFER_H_*/