#include "TRAPPCH.h"
#include "OpenGLTexture2D.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Maths/Math.h"
#include "Graphics/Textures/TextureManager.h"
#include "VFS/VFS.h"

uint32_t TRAP::Graphics::API::OpenGLTexture2D::s_maxCombinedTextureUnits = 0;

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<uint32_t, const TRAP::Graphics::API::OpenGLTexture2D*> TRAP::Graphics::API::OpenGLTexture2D::s_bound2DTextures{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = "Fallback2D";
	m_textureParameters = parameters;
	
	if(s_maxCombinedTextureUnits == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, reinterpret_cast<int32_t*>(&s_maxCombinedTextureUnits)));
	}
	
	Load();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const Image::ColorFormat format, const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = "Empty";
	m_textureParameters = parameters;
	
	if (s_maxCombinedTextureUnits == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, reinterpret_cast<int32_t*>(&s_maxCombinedTextureUnits)));
	}
	if (s_maxTextureSize == 0) //Only load maximum available texture size once
	{
		OpenGLCall(glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxTextureSize)));
	}

	if (width > s_maxTextureSize || height > s_maxTextureSize)
	{
		TP_CRITICAL("[Texture2D][OpenGL] Texture: \"", m_name, "\" Width: ", width, " or Height: ", height, " is bigger than the maximum allowed texture size(", s_maxTextureSize, ")!");
		TP_WARN("[Texture2D][OpenGL] Using Default Image!");
		const Scope<Image> image = Image::LoadFallback();
		OpenGLCall(glTextureStorage2D(m_handle, 1, TRAPImageFormatToOpenGLPrecise(image->GetColorFormat(), image->GetBytesPerPixel()), image->GetWidth(), image->GetHeight()));
		return;
	}

	InitializeTexture();

	switch (format)
	{
	case Image::ColorFormat::GrayScale:
		OpenGLCall(glTextureStorage2D(m_handle, 1, TRAPImageFormatToOpenGLPrecise(format, 1), width, height));
		break;

	case Image::ColorFormat::GrayScaleAlpha:
		OpenGLCall(glTextureStorage2D(m_handle, 1, TRAPImageFormatToOpenGLPrecise(format, 2), width, height));
		break;

	case Image::ColorFormat::RGB:
		OpenGLCall(glTextureStorage2D(m_handle, 1, TRAPImageFormatToOpenGLPrecise(format, 3), width, height));
		break;

	case Image::ColorFormat::RGBA:
		OpenGLCall(glTextureStorage2D(m_handle, 1, TRAPImageFormatToOpenGLPrecise(format, 4), width, height));
		break;

	default:
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(std::string name, const Scope<Image>& img, const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_textureParameters = parameters;

	if (img)
	{
		m_filepath = VFS::MakeVirtualPathCompatible(std::string(img->GetFilePath()));

		//Basically a version of Load() without actually Loading an TRAP::Image
		TP_DEBUG("[Texture2D][OpenGL] Loading Texture: \"", m_name, "\"");

		if (s_maxCombinedTextureUnits == 0)
		{
			OpenGLCall(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, reinterpret_cast<int32_t*>(&s_maxCombinedTextureUnits)));
		}
		if (s_maxTextureSize == 0) //Only load maximum available texture size once
		{
			OpenGLCall(glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxTextureSize)));
		}
		if (img->GetWidth() > s_maxTextureSize || img->GetHeight() > s_maxTextureSize)
		{
			TP_CRITICAL("[Texture2D][OpenGL] Texture: \"", m_name, "\" Width: ", img->GetWidth(), " or Height: ", img->GetHeight(), " is bigger than the maximum allowed texture size(", s_maxTextureSize, ")!");
			TP_WARN("[Texture2D][OpenGL] Using Default Image!");
			const Scope<Image> fallbackImg = Image::LoadFallback();
			InitializeTexture();

			UploadTexture(fallbackImg);

			return;
		}

		InitializeTexture();

		UploadTexture(img);

		return;
	}

	Scope<Image> fallbackImg = Image::LoadFallback();
	TP_CRITICAL("[Texture2D][OpenGL] Texture: \"", m_name, "\" Width: ", img->GetWidth(), " or Height: ", img->GetHeight(), " is bigger than the maximum allowed texture size(", s_maxTextureSize, ")!");
	TP_WARN("[Texture2D][OpenGL] Using Default Image!");
	fallbackImg = Image::LoadFallback();

	InitializeTexture();

	UploadTexture(fallbackImg);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepath = VFS::MakeVirtualPathCompatible(filepath);
	m_textureParameters = parameters;
	
	Load();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::~OpenGLTexture2D()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG("[Texture2D][OpenGL] Destroying Texture2D: \"", m_name, "\"");
	
	if(m_handle)
	{
		OpenGLCall(glDeleteTextures(1, &m_handle));		
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Bind(const uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
	
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
	TP_PROFILE_FUNCTION();
	
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
	TP_PROFILE_FUNCTION();

	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::SetWrap(const TextureWrap wrap)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Wrap = wrap;
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glGenerateTextureMipmap(m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::SetFilter(const TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Filter = filter;
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glGenerateTextureMipmap(m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

GLenum TRAP::Graphics::API::OpenGLTexture2D::PixelDataTypeToOpenGL(const PixelDataType type)
{
	if (type == PixelDataType::Float)
		return GL_FLOAT;
	if (type == PixelDataType::Unsigned_Short)
		return GL_UNSIGNED_SHORT;
	
	return GL_UNSIGNED_BYTE;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Load()
{
	TP_DEBUG("[Texture2D][OpenGL] Loading Texture: \"", m_name, "\"");
	Scope<Image> image;
	if (!m_filepath.empty())
		image = Image::LoadFromFile(m_filepath);
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

	InitializeTexture();

	UploadTexture(image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::InitializeTexture()
{
	OpenGLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_handle));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::UploadTexture(const Scope<Image>& image) const
{
	bool resetPixelStore = false;
	//if(!std::ispow2(image->GetWidth() || !std::ispow2(image->GetHeight())	
	if (!Math::IsPowerOfTwo(image->GetWidth()) || !Math::IsPowerOfTwo(image->GetHeight()))
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(image->GetWidth(), image->GetHeight())))); //Same as specification
	if (image->IsHDR())
		UploadData(image, numMipMapLevels, PixelDataType::Float);
	//GrayScale, GrayScaleAlpha, RGB, RGBA
	else if ((image->IsImageGrayScale() && image->GetBitsPerPixel() == 16 && !image->HasAlphaChannel()) ||
		(image->IsImageGrayScale() && image->GetBitsPerPixel() == 32 && image->HasAlphaChannel()) ||
		(image->IsImageColored() && image->GetBitsPerPixel() == 48 && !image->HasAlphaChannel()) ||
		(image->IsImageColored() && image->GetBitsPerPixel() == 64 && image->HasAlphaChannel()))
		UploadData(image, numMipMapLevels, PixelDataType::Unsigned_Short);
	else
		UploadData(image, numMipMapLevels, PixelDataType::Unsigned_Byte);

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

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::UploadData(const Scope<Image>& image, const uint32_t numMipMapLevels, const PixelDataType type) const
{
	OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(image->GetColorFormat(), image->GetBytesPerPixel()), image->GetWidth(), image->GetHeight()));

	switch(type)
	{
	case PixelDataType::Float:
		OpenGLCall(glTextureSubImage2D(m_handle, 0, 0, 0, image->GetWidth(), image->GetHeight(),
			TRAPImageFormatToOpenGL(image->GetColorFormat()), PixelDataTypeToOpenGL(type),
			TRAP::Image::FlipY(image)->GetPixelData()));
		break;

	case PixelDataType::Unsigned_Short:
		OpenGLCall(glTextureSubImage2D(m_handle, 0, 0, 0, image->GetWidth(), image->GetHeight(),
			TRAPImageFormatToOpenGL(image->GetColorFormat()), PixelDataTypeToOpenGL(type),
			TRAP::Image::FlipY(image)->GetPixelData()));
		break;

	case PixelDataType::Unsigned_Byte:
		OpenGLCall(glTextureSubImage2D(m_handle, 0, 0, 0, image->GetWidth(), image->GetHeight(),
			TRAPImageFormatToOpenGL(image->GetColorFormat()), PixelDataTypeToOpenGL(type),
			TRAP::Image::FlipY(image)->GetPixelData()));
		break;
	}	
}