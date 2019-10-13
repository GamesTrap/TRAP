#include "TRAPPCH.h"
#include "OpenGLTexture2D.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Maths/Maths.h"
#include "Graphics/Textures/TextureManager.h"
#include "VFS/VFS.h"

uint32_t TRAP::Graphics::API::OpenGLTexture2D::s_maxCombinedTextureUnits = 0;

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<uint32_t, const TRAP::Graphics::API::OpenGLTexture2D*> TRAP::Graphics::API::OpenGLTexture2D::s_bound2DTextures{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(const TextureParameters parameters)
	: m_name("Fallback2D"), m_parameters(parameters), m_handle(0)
{
	if(s_maxCombinedTextureUnits == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, reinterpret_cast<int32_t*>(&s_maxCombinedTextureUnits)));
	}
	
	Load();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(const ImageFormat format, const uint32_t width, const uint32_t height, const TextureParameters parameters)
	: m_name("Empty"), m_parameters(parameters), m_handle(0)
{
	if (s_maxCombinedTextureUnits == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, reinterpret_cast<int32_t*>(&s_maxCombinedTextureUnits)));
	}
	if (s_maxTextureSize == 0) //Only load maximum available texture size once
	{
		OpenGLCall(glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxTextureSize)));
	}

	Scope<Image> image;
	if (width > s_maxTextureSize || height > s_maxTextureSize)
	{
		TP_CRITICAL("[Texture2D][OpenGL] Texture: \"", m_name, "\" Width: ", width, " or Height: ", height, " is bigger than the maximum allowed texture size(", s_maxTextureSize, ")!");
		TP_WARN("[Texture2D][OpenGL] Using Default Image!");
		image = Image::LoadFallback();
	}
	else
		image = Image::CreateEmpty(format, width, height);

	OpenGLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_handle));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));

	OpenGLCall(glTextureStorage2D(m_handle, 1, TRAPImageFormatToOpenGLPrecise(image->GetFormat(), image->GetBytesPerPixel()), image->GetWidth(), image->GetHeight()));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
	: m_filePath(VFS::MakeVirtualPathCompatible(filepath)), m_name(std::move(name)), m_parameters(parameters), m_handle(0)
{
	Load();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::~OpenGLTexture2D()
{
	TP_DEBUG("[Texture2D][OpenGL] Destroying Texture2D: \"", m_name, "\"");
	OpenGLCall(glDeleteTextures(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Bind(const uint32_t slot) const
{
	if (s_bound2DTextures[slot] != this)
	{
		if (slot < s_maxCombinedTextureUnits)
		{
			if (m_handle)
			{
				OpenGLCall(glBindTextureUnit(slot, m_handle));
			}
			else
				TextureManager::Get("Fallback", TextureType::Texture2D)->Bind(slot);

			s_bound2DTextures[slot] = this;
		}
		else
			TP_ERROR("[Texture2D][OpenGL] Couldn't bind Texture: \"", m_name, "\" to slot: ", slot, "! Out of range");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Unbind(const uint32_t slot) const
{
	if (slot < s_maxCombinedTextureUnits)
	{
		OpenGLCall(glBindTextureUnit(slot, 0));
		s_bound2DTextures[slot] = nullptr;
	}
	else
		TP_ERROR("[Texture2D][OpenGL] Couldn't unbind Texture: \"", m_name, "\" from slot: ", slot, "! Out of range");
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLTexture2D::GetHandle() const
{
	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::OpenGLTexture2D::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

/*TRAP::Image* TRAP::Graphics::API::OpenGLTexture2D::GetImage()
{
	return m_image.get();
}*/

std::string TRAP::Graphics::API::OpenGLTexture2D::GetFilePath() const
{
	return m_filePath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters TRAP::Graphics::API::OpenGLTexture2D::GetParameters()
{
	return m_parameters;
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

void TRAP::Graphics::API::OpenGLTexture2D::SetFilter(const TextureFilter filter)
{
	m_parameters.Filter = filter;
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glGenerateTextureMipmap(m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Load()
{
	TP_DEBUG("[Texture2D][OpenGL] Loading Texture: \"", m_name, "\"");
	Scope<Image> image;
	if (!m_filePath.empty())
		image = Image::LoadFromFile(m_filePath);
	else
		image = Image::LoadFallback();

	if (s_maxCombinedTextureUnits == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, reinterpret_cast<int32_t*>(&s_maxCombinedTextureUnits)));
	}
	if (s_maxTextureSize == 0) //Only load maximum available texture size once
	{
		OpenGLCall(glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxTextureSize)));
	}
	if (image->GetWidth() > s_maxTextureSize || image->GetHeight() > s_maxTextureSize)
	{
		TP_CRITICAL("[Texture2D][OpenGL] Texture: \"", m_name, "\" Width: ", image->GetWidth(), " or Height: ", image->GetHeight(), " is bigger than the maximum allowed texture size(", s_maxTextureSize, ")!");
		TP_WARN("[Texture2D][OpenGL] Using Default Image!");
		image = Image::LoadFallback();
	}

	OpenGLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_handle));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));

	bool resetPixelStore = false;
	//if(!std::ispow2(image->GetWidth() || !std::ispow2(image->GetHeight())	
	if (!Math::IsPow2(image->GetWidth()) || !Math::IsPow2(image->GetHeight()))
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(image->GetWidth(), image->GetHeight())))); //Same as specification
	if (image->IsHDR())
	{
		OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(image->GetFormat(), image->GetBytesPerPixel()), image->GetWidth(), image->GetHeight()));
		OpenGLCall(glTextureSubImage2D(m_handle, 0, 0, 0, image->GetWidth(), image->GetHeight(),
			TRAPImageFormatToOpenGL(image->GetFormat()), GL_FLOAT,
			TRAP::Image::FlipY(image->GetWidth(), image->GetHeight(), image->GetFormat(), static_cast<float*>(image->GetPixelData())).data()));
	}
	//GrayScale, GrayScaleAlpha, RGB, RGBA
	else if ((image->IsImageGrayScale() && image->GetBitsPerPixel() == 16 && !image->HasAlphaChannel()) ||
		(image->IsImageGrayScale() && image->GetBitsPerPixel() == 24 && image->HasAlphaChannel()) ||
		(image->IsImageColored() && image->GetBitsPerPixel() == 48 && !image->HasAlphaChannel()) ||
		(image->IsImageColored() && image->GetBitsPerPixel() == 64 && image->HasAlphaChannel()))
	{
		OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(image->GetFormat(), image->GetBytesPerPixel()), image->GetWidth(), image->GetHeight()));
		OpenGLCall(glTextureSubImage2D(m_handle, 0, 0, 0, image->GetWidth(), image->GetHeight(),
			TRAPImageFormatToOpenGL(image->GetFormat()), GL_UNSIGNED_SHORT,
			TRAP::Image::FlipY(image->GetWidth(), image->GetHeight(), image->GetFormat(), static_cast<uint16_t*>(image->GetPixelData())).data()));
	}
	else
	{
		OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(image->GetFormat(), image->GetBytesPerPixel()), image->GetWidth(), image->GetHeight()));
		OpenGLCall(glTextureSubImage2D(m_handle, 0, 0, 0, image->GetWidth(), image->GetHeight(),
			TRAPImageFormatToOpenGL(image->GetFormat()), GL_UNSIGNED_BYTE,
			TRAP::Image::FlipY(image->GetWidth(), image->GetHeight(), image->GetFormat(), static_cast<uint8_t*>(image->GetPixelData())).data()));
	}

	if (image->HasAlphaChannel() && image->IsImageGrayScale())
	{
		OpenGLCall(glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_GREEN}.data()));
	}
	else if (image->IsImageGrayScale())
	{
		OpenGLCall(glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_ONE}.data()));
	}

	if (resetPixelStore)
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
	}

	OpenGLCall(glGenerateTextureMipmap(m_handle));
}