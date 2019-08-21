#include "TRAPPCH.h"
#include "OpenGLTexture2D.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Maths/Maths.h"

uint32_t TRAP::Graphics::API::OpenGLTexture2D::s_maxTextureSize = 0;

//-------------------------------------------------------------------------------------------------------------------//

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

	if(s_maxTextureSize == 0) //Only load maximum available texture size once
	{
		OpenGLCall(glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxTextureSize)));
	}
	if(m_image->GetWidth() > s_maxTextureSize || m_image->GetHeight() > s_maxTextureSize)
	{
		TP_CRITICAL("[Texture2D][OpenGL] Texture: \"", m_name, "\" Width: ", m_image->GetWidth(), " or Height: ", m_image->GetHeight(), " is bigger than the maximum allowed texture size(", s_maxTextureSize, ")!");
		TP_WARN("[Texture2D][OpenGL] Using Default Image!");
		m_image = Image::LoadFallback();
	}

	OpenGLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_handle));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));

	bool resetPixelStore = false;
	//if(!std::ispow2(m_image->GetWidth() || !std::ispow2(m_image->GetHeight())	
	if (!Math::IsPow2(m_image->GetWidth()) || !Math::IsPow2(m_image->GetHeight()))
	{
		TP_WARN("[Texture2D][OpenGL][Performance] Texture: \"", m_name, "\" is NPOT! This can affect performance!");
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(m_image->GetWidth(), m_image->GetHeight())))); //Same as specification
	if(m_image->IsHDR())
	{
		OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(m_image->GetFormat(), m_image->GetBytesPerPixel()), m_image->GetWidth(), m_image->GetHeight()));
		OpenGLCall(glTextureSubImage2D(m_handle, 0, 0, 0, m_image->GetWidth(), m_image->GetHeight(),
			           TRAPImageFormatToOpenGL(m_image->GetFormat()), GL_FLOAT, 
			           TRAP::Image::FlipY(m_image->GetWidth(), m_image->GetHeight(), m_image->GetFormat(), static_cast<float*>(m_image->GetPixelData())).data()));
	}
	//GrayScale, GrayScaleAlpha, RGB, RGBA
	else if((m_image->IsImageGrayScale() && m_image->GetBitsPerPixel() == 16 && !m_image->HasAlphaChannel()) || 
		    (m_image->IsImageGrayScale() && m_image->GetBitsPerPixel() == 24 && m_image->HasAlphaChannel()) ||
		    (m_image->IsImageColored() && m_image->GetBitsPerPixel() == 48 && !m_image->HasAlphaChannel()) || 
		    (m_image->IsImageColored() && m_image->GetBitsPerPixel() == 64 && m_image->HasAlphaChannel()))
	{
		OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(m_image->GetFormat(), m_image->GetBytesPerPixel()), m_image->GetWidth(), m_image->GetHeight()));
		OpenGLCall(glTextureSubImage2D(m_handle, 0, 0, 0, m_image->GetWidth(), m_image->GetHeight(),
			TRAPImageFormatToOpenGL(m_image->GetFormat()), GL_UNSIGNED_SHORT,
			TRAP::Image::FlipY(m_image->GetWidth(), m_image->GetHeight(), m_image->GetFormat(), static_cast<uint16_t*>(m_image->GetPixelData())).data()));
	}
	else
	{
		OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(m_image->GetFormat(), m_image->GetBytesPerPixel()), m_image->GetWidth(), m_image->GetHeight()));
		OpenGLCall(glTextureSubImage2D(m_handle, 0, 0, 0, m_image->GetWidth(), m_image->GetHeight(),
			TRAPImageFormatToOpenGL(m_image->GetFormat()), GL_UNSIGNED_BYTE,
			TRAP::Image::FlipY(m_image->GetWidth(), m_image->GetHeight(), m_image->GetFormat(), static_cast<uint8_t*>(m_image->GetPixelData())).data()));
	}
	
	if(resetPixelStore)
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
	}
	
	OpenGLCall(glGenerateTextureMipmap(m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Bind(const unsigned int slot) const
{
	OpenGLCall(glBindTextureUnit(slot, m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Unbind(const unsigned int slot) const
{
	OpenGLCall(glBindTextureUnit(0, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::SetFilter(const TextureFilter filter)
{
	m_parameters.Filter = filter;
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glGenerateTextureMipmap(m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::SetWrap(const TextureWrap wrap)
{
	m_parameters.Wrap = wrap;
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glGenerateTextureMipmap(m_handle));
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

uint32_t TRAP::Graphics::API::OpenGLTexture2D::TRAPImageFormatToOpenGLPrecise(const ImageFormat format, const uint32_t bytesPerPixel)
{
	switch (format)
	{
	case ImageFormat::RGB:
		{
		if (bytesPerPixel == 3)
			return GL_RGB8;
		if (bytesPerPixel == 6)
			return GL_RGB16;
		if (bytesPerPixel == 12)
			return GL_RGB32F;

		}

	case ImageFormat::RGBA:
		{
		if (bytesPerPixel == 4)
			return GL_RGBA8;
		if (bytesPerPixel == 8)
			return GL_RGBA16;
		if (bytesPerPixel == 16)
			return GL_RGBA32F;
		}

	case ImageFormat::Gray_Scale:
		{
		if (bytesPerPixel == 1)
			return GL_R8;
		if (bytesPerPixel == 2)
			return GL_R16;
		if (bytesPerPixel == 4)
			return GL_R32F;
		}

	case ImageFormat::Gray_Scale_Alpha:
		{
		if (bytesPerPixel == 2)
			return GL_RG8;
		if (bytesPerPixel == 4)
			return GL_RG16;
		if (bytesPerPixel == 8)
			return GL_RG32F;		
		}

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