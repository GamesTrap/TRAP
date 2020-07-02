#ifndef _TRAP_OPENGLFRAMEBUFFER2D_H_
#define _TRAP_OPENGLFRAMEBUFFER2D_H_

#include "Graphics/Buffers/FrameBuffer.h"

namespace TRAP {namespace Graphics {
	class Texture2D;
}}

namespace TRAP::Graphics::API
{
	class OpenGLFrameBuffer final : public FrameBuffer
	{
	public:
		explicit OpenGLFrameBuffer(const FrameBufferProps& props);
		virtual ~OpenGLFrameBuffer();
		OpenGLFrameBuffer(const OpenGLFrameBuffer&) = delete;
		OpenGLFrameBuffer& operator=(const OpenGLFrameBuffer&) = delete;
		OpenGLFrameBuffer(OpenGLFrameBuffer&&) = default;
		OpenGLFrameBuffer& operator=(OpenGLFrameBuffer&&) = default;

		void Invalidate();

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetColorAttachmentRendererID() const override;

		const FrameBufferProps& GetProps() const override;
		
	private:
		FrameBufferProps m_props;
		uint32_t m_handle;
		uint32_t m_colorAttachment;
		uint32_t m_depthAttachment;
		
		static uint32_t s_maxRenderBufferSize;
		static const OpenGLFrameBuffer* s_currentlyBound;
	};
}

#endif /*_TRAP_OPENGLFRAMEBUFFER2D_H_*/