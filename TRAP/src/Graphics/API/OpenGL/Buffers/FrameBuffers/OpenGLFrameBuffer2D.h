#ifndef _TRAP_OPENGLFRAMEBUFFER2D_H_
#define _TRAP_OPENGLFRAMEBUFFER2D_H_

#include "Graphics/Buffers/FrameBuffers/FrameBuffer2D.h"

namespace TRAP::Graphics::API
{
	class OpenGLFrameBuffer2D final : public FrameBuffer2D
	{
	public:
		OpenGLFrameBuffer2D(uint32_t width, uint32_t height);
		virtual ~OpenGLFrameBuffer2D();
		OpenGLFrameBuffer2D(const OpenGLFrameBuffer2D&) = delete;
		OpenGLFrameBuffer2D& operator=(const OpenGLFrameBuffer2D&) = delete;
		OpenGLFrameBuffer2D(OpenGLFrameBuffer2D&&) = default;
		OpenGLFrameBuffer2D& operator=(OpenGLFrameBuffer2D&&) = default;

		void Bind() const override;
		void Unbind() const override;
		void Clear() override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		const Scope<Texture>& GetTexture() const override;
		void SetClearColor(const Math::Vec4& color) override;
		
	private:
		void Init() override;
		
		uint32_t m_frameBufferHandle;
		uint32_t m_depthBufferHandle;

		uint32_t m_width, m_height;
		Scope<Texture> m_texture;

		bool m_valid;

		static uint32_t s_maxRenderBufferSize;
		static const OpenGLFrameBuffer2D* s_currentlyBound;
	};
}

#endif /*_TRAP_OPENGLFRAMEBUFFER2D_H_*/