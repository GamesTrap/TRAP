#include "TRAPPCH.h"

#include "OpenGLFrameBuffer2D.h"
#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Graphics/API/OpenGL/Textures/OpenGLTexture2D.h"

const TRAP::Graphics::API::OpenGLFrameBuffer2D* TRAP::Graphics::API::OpenGLFrameBuffer2D::s_currentlyBound = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLFrameBuffer2D::s_maxRenderBufferSize = 0;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLFrameBuffer2D::OpenGLFrameBuffer2D(const uint32_t width, const uint32_t height)
	: m_frameBufferHandle(0), m_depthBufferHandle(0), m_width(width), m_height(height), m_valid(false)
{
	TP_PROFILE_FUNCTION();
	
	Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLFrameBuffer2D::~OpenGLFrameBuffer2D()
{
	TP_PROFILE_FUNCTION();
	
	if (m_valid && s_currentlyBound == this)
	{
		Unbind();
		if (m_frameBufferHandle)
		{
			OpenGLCall(glDeleteFramebuffers(1, &m_frameBufferHandle));
		}
		if (m_depthBufferHandle)
		{
			OpenGLCall(glDeleteRenderbuffers(1, &m_depthBufferHandle));
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLFrameBuffer2D::Bind() const
{
	TP_PROFILE_FUNCTION();
	
	if (m_valid && s_currentlyBound != this)
	{
		OpenGLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle));
		s_currentlyBound = this;
		OpenGLCall(glViewport(0, 0, m_width, m_height));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLFrameBuffer2D::Unbind() const
{
	TP_PROFILE_FUNCTION();
	
	if (s_currentlyBound)
	{
		OpenGLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		s_currentlyBound = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLFrameBuffer2D::Clear()
{
	TP_PROFILE_FUNCTION();
	
	if (m_valid)
	{
		if (s_currentlyBound == this)
		{
			OpenGLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		}
		else
		{
			Bind();
			OpenGLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			Unbind();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLFrameBuffer2D::GetWidth() const
{
	TP_PROFILE_FUNCTION();
	
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLFrameBuffer2D::GetHeight() const
{
	TP_PROFILE_FUNCTION();
	
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::Texture>& TRAP::Graphics::API::OpenGLFrameBuffer2D::GetTexture() const
{
	TP_PROFILE_FUNCTION();
	
	return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLFrameBuffer2D::SetClearColor(const Math::Vec4& color)
{
	TP_PROFILE_FUNCTION();
	
	if (m_valid)
	{
		if (s_currentlyBound == this)
		{
			OpenGLCall(glClearColor(color.x, color.y, color.z, color.w));
		}
		else
		{
			Bind();
			OpenGLCall(glClearColor(color.x, color.y, color.z, color.w));
			Unbind();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLFrameBuffer2D::Init()
{	
	if (s_maxRenderBufferSize == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, reinterpret_cast<int32_t*>(&s_maxRenderBufferSize)));
	}

	if(m_width > s_maxRenderBufferSize || m_height > s_maxRenderBufferSize)
	{
		TP_ERROR("[FrameBuffer2D][OpenGL] Couldn't create FrameBuffer!");
		TP_ERROR("[FrameBuffer2D][OpenGL] Width: ", m_width, " or Height: ", m_height, " is bigger than the maximum render buffer size: ", s_maxRenderBufferSize, "!");
		return;
	}

	OpenGLCall(glCreateFramebuffers(1, &m_frameBufferHandle));
	OpenGLCall(glCreateRenderbuffers(1, &m_depthBufferHandle));

	m_texture = Texture2D::CreateEmpty(m_width, m_height, 32, Image::ColorFormat::RGBA, { TextureFilter::Linear, TextureWrap::Clamp_To_Edge });
	if (m_texture)
	{
		OpenGLCall(glNamedFramebufferTexture(m_frameBufferHandle, GL_COLOR_ATTACHMENT0, (dynamic_cast<OpenGLTexture2D*>(m_texture.get()))->GetHandle(), 0));

		OpenGLCall(glNamedRenderbufferStorage(m_depthBufferHandle, GL_DEPTH_COMPONENT24, m_width, m_height));
		OpenGLCall(glNamedFramebufferRenderbuffer(m_frameBufferHandle, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferHandle));

		OpenGLCall(glCheckNamedFramebufferStatus(m_frameBufferHandle, GL_FRAMEBUFFER));
	}
}