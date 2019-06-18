#include "TRAPPCH.h"
#include "OpenGLTexture2D.h"

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(const TextureParameters parameters)
	: m_name("Fallback"), m_parameters(parameters), m_handle(0)
{
	Load("");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
	: m_name(std::move(name)), m_parameters(parameters), m_handle(0)
{
	Load(filepath);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::~OpenGLTexture2D()
{
	TP_DEBUG("[Texture2D][OpenGL] Destroying Texture2D: \"", m_name, "\"");
	OpenGLCall(glDeleteTextures(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Load(const std::string& filepath)
{
	if (!filepath.empty())
	{
		m_image = std::make_unique<Image>(filepath);
		m_parameters.Format = m_image->GetBitsPerPixel() == 24 ? TextureFormat::RGB : TextureFormat::RGBA;
	}
	else
		m_image = std::make_unique<Image>();

	OpenGLCall(glGenTextures(1, &m_handle));
	OpenGLCall(glBindTexture(GL_TEXTURE_2D, m_handle));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(s_WrapMode)));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(s_WrapMode)));

	OpenGLCall(glTexImage2D(GL_TEXTURE_2D, 0, TRAPTextureFormatToOpenGL(m_parameters.Format), m_image->GetWidth(), m_image->GetHeight(), 0, TRAPTextureFormatToOpenGL(m_parameters.Format), GL_UNSIGNED_BYTE, m_image->GetData().data()));
	OpenGLCall(glGenerateMipmap(GL_TEXTURE_2D));
	OpenGLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Bind(const unsigned int slot) const
{
	OpenGLCall(glActiveTexture(GL_TEXTURE0 + slot));
	OpenGLCall(glBindTexture(GL_TEXTURE_2D, m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Unbind(const unsigned int slot) const
{
	OpenGLCall(glActiveTexture(GL_TEXTURE0 + slot));
	OpenGLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLTexture2D::TRAPTextureFormatToOpenGL(const TextureFormat format)
{
	switch(format)
	{
	case TextureFormat::RGBA:
		return GL_RGBA;

	case TextureFormat::RGB:
		return GL_RGB;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLTexture2D::TRAPTextureWrapToOpenGL(const TextureWrap wrap)
{
	switch(wrap)
	{
	case TextureWrap::CLAMP_TO_BORDER:
		return GL_CLAMP_TO_BORDER;

	case TextureWrap::CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;

	case TextureWrap::REPEAT:
		return GL_REPEAT;

	case TextureWrap::MIRRORED_REPEAT:
		return GL_MIRRORED_REPEAT;

	default:
		return 0;
	}
}