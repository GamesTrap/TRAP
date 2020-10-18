#include "TRAPPCH.h"
#include "OpenGLTexture2D.h"

#include "Application.h"
#include "Graphics/API/OpenGL/OpenGLRenderer.h"
#include "Maths/Math.h"
#include "Graphics/Textures/TextureManager.h"
#include "VFS/VFS.h"

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(const TextureParameters parameters)
	: m_handle(0), m_width(0), m_height(0), m_bitsPerPixel(0), m_colorFormat(Image::ColorFormat::NONE)
{
	TP_PROFILE_FUNCTION();

	m_name = "Fallback2D";
	m_textureParameters = parameters;
	
	TP_DEBUG(Log::Texture2DOpenGLPrefix, "Loading Texture: \"", m_name, "\"");
	Scope<Image> image;
	if (!m_filepath.empty())
	{
		image = Image::LoadFromFile(m_filepath);
		SetImageData(image);

		if (!CheckLimits())
			return;

		InitializeTexture();
		UploadTexture(image, false);
	}
	else
	{
		image = Image::LoadFallback();
		UploadFallback();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const Image::ColorFormat format, const TextureParameters parameters)
	: m_handle(0), m_width(width), m_height(height), m_bitsPerPixel(bitsPerPixel), m_colorFormat(format)
{
	TP_PROFILE_FUNCTION();

	m_name = "Empty";
	m_textureParameters = parameters;

	if (!CheckLimits())
		return;

	InitializeTexture();

	glTextureStorage2D(m_handle, 1, TRAPImageFormatToOpenGLPrecise(format, bitsPerPixel / 8), width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(std::string name, const Scope<Image>& img, const TextureParameters parameters)
	: m_handle(0), m_width(0), m_height(0), m_bitsPerPixel(0), m_colorFormat(Image::ColorFormat::NONE)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_textureParameters = parameters;

	if (img)
	{
		SetImageData(img);
		m_filepath = VFS::MakeVirtualPathCompatible(img->GetFilePath());

		TP_DEBUG(Log::Texture2DOpenGLPrefix, "Loading Texture: \"", m_name, "\"");

		if (!CheckLimits())
			return;

		InitializeTexture();
		UploadTexture(img, false);

		return;
	}

	TP_CRITICAL(Log::Texture2DOpenGLPrefix, "Texture: Provided TRAP::Image is a nullptr!");
	UploadFallback();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::OpenGLTexture2D(std::string name, const std::string_view filepath, const TextureParameters parameters)
	: m_handle(0), m_width(0), m_height(0), m_bitsPerPixel(0), m_colorFormat(Image::ColorFormat::NONE)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepath = VFS::MakeVirtualPathCompatible(filepath);
	m_textureParameters = parameters;

	TP_DEBUG(Log::Texture2DOpenGLPrefix, "Initializing Texture: \"", m_name, "\"");
	InitializeTexture();

	if (!m_filepath.empty())
		m_loadingTextures.emplace_back(this, Application::GetThreadPool().EnqueueTask(Image::LoadFromFile, std::string(m_filepath)));
	else
	{
		Scope<Image> image = Image::LoadFallback();
		UploadFallback();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTexture2D::~OpenGLTexture2D()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG(Log::Texture2DOpenGLPrefix, "Destroying Texture2D: \"", m_name, "\"");
	
	if(m_handle)
		glDeleteTextures(1, &m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Bind(const uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
	
	if (OpenGLRenderer::GetBoundTextures()[slot] != this)
	{
		if (slot < OpenGLRenderer::GetMaxTextureUnits())
		{
			if (m_handle)
				glBindTextureUnit(slot, m_handle);
			else
				TextureManager::Get("Fallback", TextureType::Texture2D)->Bind(slot);

			OpenGLRenderer::GetBoundTextures()[slot] = this;
		}
		else
			TP_ERROR(Log::Texture2DOpenGLPrefix, "Couldn't bind Texture: \"", m_name, "\" to slot: ", slot, "! Out of range");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::Unbind(const uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
	
	if (slot < OpenGLRenderer::GetMaxTextureUnits())
	{
		glBindTextureUnit(slot, 0);
		OpenGLRenderer::GetBoundTextures()[slot] = nullptr;
	}
	else
		TP_ERROR(Log::Texture2DOpenGLPrefix, "Couldn't unbind Texture: \"", m_name, "\" from slot: ", slot, "! Out of range");
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
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(wrap));
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(wrap));
	glGenerateTextureMipmap(m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::SetFilter(const TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Filter = filter;
	glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
	glGenerateTextureMipmap(m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::UploadImage(const TRAP::Scope<TRAP::Image>& image)
{
	if (image)
	{
		//Check if multi threaded loading finished
		if (m_handle && m_filepath == image->GetFilePath() && m_width == 0 && m_height == 0 && m_bitsPerPixel == 0 && m_colorFormat == Image::ColorFormat::NONE)
		{
			TP_DEBUG(Log::Texture2DOpenGLPrefix, "Uploading Texture: \"", m_name, "\"");
			SetImageData(image);

			if (!CheckLimits())
				return;

			UploadTexture(image, false);

			return;
		}

		if ((m_width != image->GetWidth() || m_height != image->GetHeight() || m_bitsPerPixel != image->GetBitsPerPixel() || m_colorFormat != image->GetColorFormat()) && m_handle)
		{
			//Performace warning because texture gets recreated
			TP_WARN(Log::Texture2DOpenGLPerformancePrefix, "Texture: \"", m_name, "\" image upload uses mismatching width, height, bits per pixel and/or color format!");
			
			//Delete current texture if exists
			if (m_handle)
				glDeleteTextures(1, &m_handle);

			SetImageData(image);
			
			InitializeTexture();
		}
		m_filepath = "";

		if (!CheckLimits())
			return;

		UploadTexture(image, true);
	}
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

void TRAP::Graphics::API::OpenGLTexture2D::InitializeTexture()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);
	glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap));
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::UploadTexture(const Scope<Image>& image, const bool exists) const
{
	bool resetPixelStore = false;
	//if(!std::ispow2(image->GetWidth() || !std::ispow2(image->GetHeight())	
	if (!Math::IsPowerOfTwo(image->GetWidth()) || !Math::IsPowerOfTwo(image->GetHeight()))
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(image->GetWidth(), image->GetHeight())))); //Same as specification
	if(!exists)
		glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(image->GetColorFormat(), image->GetBytesPerPixel()), image->GetWidth(), image->GetHeight());
	if (image->IsHDR())
		UploadData(image, PixelDataType::Float);
	//GrayScale, GrayScaleAlpha, RGB, RGBA
	else if ((image->IsImageGrayScale() && image->GetBitsPerPixel() == 16 && !image->HasAlphaChannel()) ||
			 (image->IsImageGrayScale() && image->GetBitsPerPixel() == 32 && image->HasAlphaChannel()) ||
		     (image->IsImageColored() && image->GetBitsPerPixel() == 48 && !image->HasAlphaChannel()) ||
		     (image->IsImageColored() && image->GetBitsPerPixel() == 64 && image->HasAlphaChannel()))
		UploadData(image, PixelDataType::Unsigned_Short);
	else
		UploadData(image, PixelDataType::Unsigned_Byte);

	if (image->HasAlphaChannel() && image->IsImageGrayScale())
		glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_GREEN}.data());
	else if (image->IsImageGrayScale())
		glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_ONE}.data());

	if (resetPixelStore)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glGenerateTextureMipmap(m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::UploadData(const Scope<Image>& image, const PixelDataType type) const
{
	glTextureSubImage2D(m_handle, 0, 0, 0, image->GetWidth(), image->GetHeight(),
		TRAPImageFormatToOpenGL(image->GetColorFormat()), PixelDataTypeToOpenGL(type),
		TRAP::Image::FlipY(image)->GetPixelData());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::UploadFallback()
{
	TP_WARN(Log::Texture2DOpenGLPrefix, "Using Default Image!");
	const Scope<Image> image = Image::LoadFallback();
	SetImageData(image);
	if(m_handle == 0)
		InitializeTexture();

	UploadTexture(image, false);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLTexture2D::CheckLimits()
{
	if (s_maxTextureSize == 0) //Only load maximum available texture size once
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxTextureSize));

	if (m_width > s_maxTextureSize || m_height > s_maxTextureSize)
	{
		TP_CRITICAL(Log::Texture2DOpenGLPrefix, "Texture: \"", m_name, "\" Width: ", m_width, " or Height: ", m_height, " is bigger than the maximum allowed texture size(", s_maxTextureSize, ")!");
		UploadFallback();
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTexture2D::SetImageData(const Scope<Image>& image)
{
	m_width = image->GetWidth();
	m_height = image->GetHeight();
	m_bitsPerPixel = image->GetBitsPerPixel();
	m_colorFormat = image->GetColorFormat();
}