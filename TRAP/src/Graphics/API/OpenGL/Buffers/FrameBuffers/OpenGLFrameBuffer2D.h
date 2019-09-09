#ifndef _TRAP_OPENGLFRAMEBUFFER2D_H_
#define _TRAP_OPENGLFRAMEBUFFER2D_H_

#include "Graphics/API/OpenGL/Textures/OpenGLTexture2D.h"
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

		const std::unique_ptr<Texture>& GetTexture() const override;
		void SetClearColor(const Math::Vec4& color) override;
		
	private:
		void Init() override;
		
		uint32_t m_frameBufferHandle;
		uint32_t m_depthBufferHandle;

		uint32_t m_width, m_height;
		std::unique_ptr<Texture2D> m_texture;

		static const OpenGLFrameBuffer2D* s_currentlyBound;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::API::OpenGLFrameBuffer2D::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::API::OpenGLFrameBuffer2D::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::unique_ptr<TRAP::Graphics::Texture>& TRAP::Graphics::API::OpenGLFrameBuffer2D::GetTexture() const
{
	return reinterpret_cast<const std::unique_ptr<Texture>&>(m_texture);
}

#endif /*_TRAP_OPENGLFRAMEBUFFER2D_H_*/