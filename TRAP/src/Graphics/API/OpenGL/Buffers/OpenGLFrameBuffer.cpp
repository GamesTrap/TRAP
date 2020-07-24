#include "TRAPPCH.h"

#include "OpenGLFrameBuffer.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"

const TRAP::Graphics::API::OpenGLFrameBuffer* TRAP::Graphics::API::OpenGLFrameBuffer::s_currentlyBound = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLFrameBuffer::s_maxRenderBufferSize = 0;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferProps& props)
	: m_props(props), m_handle(0), m_colorAttachment(0), m_depthAttachment(0)
{
	TP_PROFILE_FUNCTION();

	Invalidate();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	TP_PROFILE_FUNCTION();

	if(s_currentlyBound == this)
		Unbind();
	
	if(m_handle)
	{
		OpenGLCall(glDeleteFramebuffers(1, &m_handle));
	}
	if(m_depthAttachment)
	{
		OpenGLCall(glDeleteTextures(1, &m_depthAttachment));
	}
	if(m_colorAttachment)
	{
		OpenGLCall(glDeleteTextures(2, &m_colorAttachment))
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLFrameBuffer::Invalidate()
{
	TP_PROFILE_FUNCTION();
	
	if (s_maxRenderBufferSize == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, reinterpret_cast<int32_t*>(&s_maxRenderBufferSize)));
	}

	if (m_props.Width > s_maxRenderBufferSize || m_props.Height > s_maxRenderBufferSize)
	{
		TP_ERROR(Log::FrameBufferOpenGLPrefix, "Couldn't create FrameBuffer!");
		TP_ERROR(Log::FrameBufferOpenGLPrefix, "Width: ", m_props.Width, " or Height: ", m_props.Height, " is bigger than the maximum render buffer size: ", s_maxRenderBufferSize, "!");
		return;
	}


	if (m_handle)
	{
		OpenGLCall(glDeleteFramebuffers(1, &m_handle));
		if (m_depthAttachment)
		{
			OpenGLCall(glDeleteTextures(1, &m_depthAttachment));
		}
		if (m_colorAttachment)
		{
			OpenGLCall(glDeleteTextures(2, &m_colorAttachment))
		}
	}
	
	OpenGLCall(glCreateFramebuffers(1, &m_handle));

	//Color Attachment
	//Create Texture
	OpenGLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment));
	OpenGLCall(glTextureParameteri(m_colorAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	OpenGLCall(glTextureParameteri(m_colorAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	OpenGLCall(glTextureStorage2D(m_colorAttachment, 1, GL_RGBA8, m_props.Width, m_props.Height));
	//Attach Texture
	OpenGLCall(glNamedFramebufferTexture(m_handle, GL_COLOR_ATTACHMENT0, m_colorAttachment, 0));

	//Depth Attachment
	//Create Texture
	OpenGLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment));
	OpenGLCall(glTextureParameteri(m_depthAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	OpenGLCall(glTextureParameteri(m_depthAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	OpenGLCall(glTextureStorage2D(m_depthAttachment, 1, GL_DEPTH24_STENCIL8, m_props.Width, m_props.Height));
	//Attach Texture
	OpenGLCall(glNamedFramebufferTexture(m_handle, GL_DEPTH_STENCIL_ATTACHMENT, m_depthAttachment, 0));

	//Check FrameBuffer
	OpenGLCall(glCheckNamedFramebufferStatus(m_handle, GL_FRAMEBUFFER));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLFrameBuffer::Bind()
{
	TP_PROFILE_FUNCTION();

	if(s_currentlyBound != this)
	{
		OpenGLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_handle));
		s_currentlyBound = this;
		OpenGLCall(glViewport(0, 0, m_props.Width, m_props.Height));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLFrameBuffer::Unbind()
{
	TP_PROFILE_FUNCTION();

	if(s_currentlyBound)
	{
		OpenGLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		s_currentlyBound = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLFrameBuffer::Resize(const uint32_t width, const uint32_t height)
{
	m_props.Width = width;
	m_props.Height = height;
	Invalidate();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLFrameBuffer::GetColorAttachmentRendererID() const
{
	return m_colorAttachment;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::FrameBufferProps& TRAP::Graphics::API::OpenGLFrameBuffer::GetProps() const
{
	return m_props;
}