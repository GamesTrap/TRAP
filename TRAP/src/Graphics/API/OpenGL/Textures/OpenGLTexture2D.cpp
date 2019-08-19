#include "TRAPPCH.h"
#include "OpenGLTexture2D.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Maths/Maths.h"

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(const TextureParameters parameters)
	: m_name("Fallback"), m_parameters(parameters), m_handle(0), parameters(parameters)
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
	TP_DEBUG("[Texture2D][OpenGL] Loading Texture: \"", m_name, "\"");
	if (!filepath.empty())
		m_image = Image::LoadFromFile(filepath);
	else
		m_image = Image::LoadFallback();

	int maxTextureSize = 0;
	OpenGLCall(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize));
	if(m_image->GetWidth() > static_cast<unsigned int>(maxTextureSize) || static_cast<unsigned int>(m_image->GetHeight()) > static_cast<unsigned int>(maxTextureSize))
	{
		TP_CRITICAL("[Texture2D][OpenGL] Width: ", m_image->GetWidth(), " or Height: ", m_image->GetHeight(), " is bigger than the maximum allowed texture size(", maxTextureSize, ")!");
		TP_WARN("[Texture2D][OpenGL] Using Default Image!");
		m_image = Image::LoadFallback();
	}

	OpenGLCall(glGenTextures(1, &m_handle));
	OpenGLCall(glBindTexture(GL_TEXTURE_2D, m_handle));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));

	bool resetPixelStore = false;
	//if(!std::ispow2(m_image->GetWidth() || !std::ispow2(m_image->GetHeight())
	if (!Math::IsPow2(m_image->GetWidth()) || !Math::IsPow2(m_image->GetHeight()))
	{
		TP_WARN("[Texture2D][OpenGL][Performance] Image is NPOT! This can affect transfer speed!");
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}
	
	if(m_image->IsHDR())
	{
		OpenGLCall(glTexImage2D(GL_TEXTURE_2D, 0, TRAPImageFormatToOpenGL(m_image->GetFormat()), m_image->GetWidth(),
			                    m_image->GetHeight(), 0, TRAPImageFormatToOpenGL(m_image->GetFormat()), GL_FLOAT,
			                    TRAP::Image::FlipY(m_image->GetWidth(), m_image->GetHeight(), m_image->GetFormat(), static_cast<float*>(m_image->GetPixelData())).data()));
	}
	//GrayScale, GrayScaleAlpha, RGB, RGBA
	else if((m_image->IsImageGrayScale() && m_image->GetBitsPerPixel() == 16 && !m_image->HasAlphaChannel()) || 
		    (m_image->IsImageGrayScale() && m_image->GetBitsPerPixel() == 24 && m_image->HasAlphaChannel()) ||
		    (m_image->IsImageColored() && m_image->GetBitsPerPixel() == 48 && !m_image->HasAlphaChannel()) || 
		    (m_image->IsImageColored() && m_image->GetBitsPerPixel() == 64 && m_image->HasAlphaChannel()))
	{
		OpenGLCall(glTexImage2D(GL_TEXTURE_2D, 0, TRAPImageFormatToOpenGL(m_image->GetFormat()), m_image->GetWidth(),
			       m_image->GetHeight(), 0, TRAPImageFormatToOpenGL(m_image->GetFormat()), GL_UNSIGNED_SHORT,
			       TRAP::Image::FlipY(m_image->GetWidth(), m_image->GetHeight(), m_image->GetFormat(), static_cast<uint16_t*>(m_image->GetPixelData())).data()));
	}
	else
	{
		OpenGLCall(glTexImage2D(GL_TEXTURE_2D, 0, TRAPImageFormatToOpenGL(m_image->GetFormat()), m_image->GetWidth(),
			                    m_image->GetHeight(), 0, TRAPImageFormatToOpenGL(m_image->GetFormat()), GL_UNSIGNED_BYTE,
							    TRAP::Image::FlipY(m_image->GetWidth(), m_image->GetHeight(), m_image->GetFormat(), static_cast<uint8_t*>(m_image->GetPixelData())).data()));
	}

	OpenGLCall(glGenerateMipmap(GL_TEXTURE_2D));
	OpenGLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if(resetPixelStore)
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
	}
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

void TRAP::Graphics::API::OpenGLTexture2D::SetFilter(const TextureFilter filter)
{
	m_parameters.Filter = filter;
	Bind();
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glGenerateMipmap(GL_TEXTURE_2D));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::SetWrap(const TextureWrap wrap)
{
	m_parameters.Wrap = wrap;
	Bind();
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glGenerateMipmap(GL_TEXTURE_2D));
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLTexture2D::TRAPImageFormatToOpenGL(const ImageFormat format)
{
	switch(format)
	{
	case ImageFormat::RGB:
		return GL_RGB;

	case ImageFormat::RGBA:
		return GL_RGBA;

	case ImageFormat::Gray_Scale:
		return GL_RED;

	case ImageFormat::Gray_Scale_Alpha:
		return GL_RG;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLTexture2D::TRAPTextureWrapToOpenGL(const TextureWrap wrap)
{
	switch(wrap)
	{
	case TextureWrap::Clamp_To_Border:
		return GL_CLAMP_TO_BORDER;

	case TextureWrap::Clamp_To_Edge:
		return GL_CLAMP_TO_EDGE;

	case TextureWrap::Repeat:
		return GL_REPEAT;

	case TextureWrap::Mirrored_Repeat:
		return GL_MIRRORED_REPEAT;

	default:
		return 0;
	}
}