#include "TRAPPCH.h"
#include "OpenGLTextureCube.h"

#include "Graphics/API/OpenGL/OpenGLRenderer.h"
#include "Graphics/Textures/TextureManager.h"
#include "VFS/VFS.h"
#include "Maths/Math.h"

TRAP::Graphics::API::OpenGLTextureCube::OpenGLTextureCube(const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = "FallbackCube";
	m_textureParameters = parameters;
	m_inputFormat = InputFormat::NONE;
	
	TP_DEBUG(Log::TextureCubeOpenGLPrefix, "Loading Texture: \"", m_name, "\"");
	if (s_maxCubeTextureSize == 0) //Only load maximum available texture size once
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize));
	Scope<Image> image = Image::LoadFallback();
	image = Image::FlipY(image);

	InitializeTexture();

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(image->GetWidth(), image->GetHeight())))); //Same as specification
	glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(image->GetColorFormat(), image->GetBytesPerPixel()), image->GetWidth(), image->GetHeight());

	for(uint32_t i = 0; i < 6; i++)
		glTextureSubImage3D(m_handle, 0, 0, 0, i, image->GetWidth(), image->GetHeight(), 1,
			TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_UNSIGNED_BYTE,	image->GetPixelData());

	glGenerateTextureMipmap(m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::OpenGLTextureCube(std::string name, const std::array<std::string_view, 6>& filepaths, const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	for(uint32_t i = 0; i < filepaths.size(); i++)
		m_filepaths[i] = VFS::MakeVirtualPathCompatible(filepaths[i]);
	m_textureParameters = parameters;
	m_inputFormat = InputFormat::NONE;
	
	TP_DEBUG(Log::TextureCubeOpenGLPrefix, "Loading Texture: \"", m_name, "\"");
	if (s_maxCubeTextureSize == 0) //Only load maximum available texture size once
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize));

	for (const auto& path : m_filepaths)
	{
		if (path.empty())
		{
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "Missing File(s)!");
			TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
			return;
		}
	}

	LoadFiles();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::OpenGLTextureCube(std::string name, const std::string_view filepath, const InputFormat format, const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepaths[0] = VFS::MakeVirtualPathCompatible(filepath);
	m_textureParameters = parameters;
	m_inputFormat = format;
	
	TP_DEBUG(Log::TextureCubeOpenGLPrefix, "Loading Texture: \"", m_name, "\"");
	if (s_maxCubeTextureSize == 0) //Only load maximum available texture size once
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize));

	if (m_filepaths[0].empty())
	{
		TP_ERROR(Log::TextureCubeOpenGLPrefix, "Filepath is empty!");
		TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
		return;
	}

	if (m_inputFormat == InputFormat::Vertical_Cross)
		SetupCross();
	else if (m_inputFormat == InputFormat::Horizontal_Cross)
		SetupCross();
	else if (m_inputFormat == InputFormat::NONE)
	{
		TP_ERROR(Log::TextureCubeOpenGLPrefix, "InputFormat is None!");
		TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
		return;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::OpenGLTextureCube(std::string name, const Scope<Image>& img, const InputFormat format, const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();
	
	m_name = std::move(name);
	m_textureParameters = parameters;
	m_inputFormat = format;

	if (img)
	{
		m_filepaths[0] = VFS::MakeVirtualPathCompatible(img->GetFilePath());

		TP_DEBUG(Log::TextureCubeOpenGLPrefix, "Loading Texture: \"", m_name, "\"");
		if (s_maxCubeTextureSize == 0) //Only load maximum available texture size once
			glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize));

		if(!CheckImageSize(img))
		{
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "Filepath is empty!");
			TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
			return;
		}
		if(m_inputFormat == InputFormat::NONE)
		{
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "No Input Format given!");
			TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
			return;
		}

		if(m_inputFormat == InputFormat::Vertical_Cross)
		{
			if (img->GetHeight() <= img->GetWidth())
			{
				TP_ERROR(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Invalid InputFormat usage!");
				TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
				return;
			}

			LoadCross(img);
		}
		else
		{
			if (img->GetWidth() <= img->GetHeight())
			{
				TP_ERROR(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Invalid InputFormat usage!");
				TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
				return;
			}

			LoadCross(img);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::~OpenGLTextureCube()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG(Log::TextureCubeOpenGLPrefix, "Destroying TextureCube: \"", m_name, "\"");
	
	if(m_handle)
		glDeleteTextures(1, &m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::Bind(const uint32_t slot) const
{
	TP_PROFILE_FUNCTION();

	if(OpenGLRenderer::GetBoundTextures()[slot] != this)
	{
		if (slot < OpenGLRenderer::GetMaxTextureUnits())
		{
			if (m_handle)
				glBindTextureUnit(slot, m_handle);
			else
				TextureManager::Get("FallbackCube", TextureType::TextureCube)->Bind(slot);

			OpenGLRenderer::GetBoundTextures()[slot] = this;
		}
		else
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "Couldn't bind Texture: \"", m_name, "\" to slot: ", slot, "! Out of range");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::Unbind(const uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
	
	if (slot < OpenGLRenderer::GetMaxTextureUnits())
	{
		glBindTextureUnit(slot, 0);
		OpenGLRenderer::GetBoundTextures()[slot] = nullptr;
	}
	else
		TP_ERROR(Log::TextureCubeOpenGLPrefix, "Couldn't unbind Texture: \"", m_name, "\" from slot: ", slot, "! Out of range");
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLTextureCube::GetHandle() const
{
	TP_PROFILE_FUNCTION();

	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::SetWrap(const TextureWrap wrap)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Wrap = wrap;
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(wrap));
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(wrap));
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(wrap));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::SetFilter(const TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Filter = filter;
	glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::SetupCross()
{
	std::filesystem::path physicalPath;
	if (!VFS::ResolveReadPhysicalPath(m_filepaths[0], physicalPath, true))
	{
		TP_ERROR(Log::TextureCubeOpenGLPrefix, "Couldn't resolve FilePath: ", m_filepaths[0], "!");
		TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map!");
		return;
	}

	const Scope<Image> image = Image::LoadFromFile(m_filepaths[0]);

	if (!CheckImageSize(image))
		return;
	if(m_inputFormat == InputFormat::Horizontal_Cross)
	{
		if (image->GetWidth() <= image->GetHeight())
		{
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Invalid InputFormat usage!");
			TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
			return;
		}
	}
	else if(m_inputFormat == InputFormat::Vertical_Cross)
	{
		if (image->GetHeight() <= image->GetWidth())
		{
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Invalid InputFormat usage!");
			TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
			return;
		}
	}	

	LoadCross(image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::LoadCross(const Scope<Image>& img)
{
	const uint32_t stride = img->GetBitsPerPixel() / 8;
	uint32_t face = 0;
	uint32_t faceWidth = img->GetWidth() / 3, faceHeight = img->GetHeight() / 4;
	if(m_inputFormat == InputFormat::Horizontal_Cross)
	{
		faceWidth = img->GetWidth() / 4;
		faceHeight = img->GetHeight() / 3;
	}

	bool resetPixelStore = false;
	InitializeTexture();

	//if (!std::ispow2(faceWidth) || !std::ispow2(faceHeight))
	if (!Math::IsPowerOfTwo(faceWidth) || !Math::IsPowerOfTwo(faceHeight))
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(faceWidth, faceHeight)))); //Same as specification
	glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(img->GetColorFormat(), img->GetBytesPerPixel()), faceWidth, faceHeight);

	UploadImage(img, faceWidth, faceHeight, face, stride);

	if (img->HasAlphaChannel() && img->IsImageGrayScale())
		glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_GREEN}.data());
	else if (img->IsImageGrayScale())
		glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_ONE}.data());

	if (resetPixelStore)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glGenerateTextureMipmap(m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::LoadFiles()
{
	std::array<Scope<Image>, 6> images{};
	for (uint32_t i = 0; i < m_filepaths.size(); i++)
	{
		std::filesystem::path physicalPath;
		if (!VFS::ResolveReadPhysicalPath(m_filepaths[i], physicalPath, true))
		{
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "Couldn't resolve FilePath: ", m_filepaths[i], "!");
			TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map!");
			return;
		}

		images[i] = Image::LoadFromFile(m_filepaths[i]);
	}

	const uint32_t width = images[0]->GetWidth();
	const uint32_t height = images[0]->GetHeight();
	const bool isGrayScale = images[0]->IsImageGrayScale();
	const bool hasAlphaChannel = images[0]->HasAlphaChannel();

	for (const auto& image : images)
	{
		if (!CheckImageSize(image))
			return;

		if (image->GetWidth() != width || image->GetHeight() != height)
		{
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Image size: ", image->GetWidth(), "x", image->GetHeight(), " is not the same size as the other images!");
			TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
			return;
		}

		if(image->IsImageGrayScale() != isGrayScale && image->HasAlphaChannel() != hasAlphaChannel)
		{
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Image: \"", image->GetFilePath(), "\" doesn't match other images properties!");
			TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
			return;
		}
	}

	bool resetPixelStore = false;
	InitializeTexture();

	//if (!std::ispow2(faceWidth) || !std::ispow2(faceHeight))
	if (!Math::IsPowerOfTwo(width) || !Math::IsPowerOfTwo(height))
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(width, height)))); //Same as specification
	glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(images[0]->GetColorFormat(), images[0]->GetBytesPerPixel()), width, height);

	for (uint32_t i = 0; i < 6; i++)
	{
		if (images[i]->IsHDR())
			glTextureSubImage3D(m_handle, 0, 0, 0, i, width, height, 1, TRAPImageFormatToOpenGL(images[0]->GetColorFormat()), GL_FLOAT, images[i]->GetPixelData());
		else if ((images[i]->IsImageGrayScale() && images[i]->GetBitsPerPixel() == 16 && !images[i]->HasAlphaChannel()) ||
			(images[i]->IsImageGrayScale() && images[i]->GetBitsPerPixel() == 32 && images[i]->HasAlphaChannel()) ||
			(images[i]->IsImageColored() && images[i]->GetBitsPerPixel() == 48 && !images[i]->HasAlphaChannel()) ||
			(images[i]->IsImageColored() && images[i]->GetBitsPerPixel() == 64 && images[i]->HasAlphaChannel()))
		{
			glTextureSubImage3D(m_handle, 0, 0, 0, i, width, height, 1, TRAPImageFormatToOpenGL(images[i]->GetColorFormat()), GL_UNSIGNED_SHORT, images[i]->GetPixelData());
		}
		else
			glTextureSubImage3D(m_handle, 0, 0, 0, i, width, height, 1, TRAPImageFormatToOpenGL(images[i]->GetColorFormat()), GL_UNSIGNED_BYTE, images[i]->GetPixelData());
	}

	if (hasAlphaChannel && isGrayScale)
		glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_GREEN}.data());
	else if (isGrayScale)
		glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_ONE}.data());
	
	if (resetPixelStore)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glGenerateTextureMipmap(m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLTextureCube::CheckImageSize(const Scope<Image>& image) const
{
	if (image->GetWidth() > s_maxCubeTextureSize || image->GetHeight() > s_maxCubeTextureSize)
	{
		TP_CRITICAL(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Width: ", image->GetWidth(), " or Height: ", image->GetHeight(), " is bigger than the maximum allowed texture size(", s_maxCubeTextureSize, ")!");
		TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::InitializeTexture()
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_handle);
	glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap));
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap));
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
std::array<std::vector<T>, 6> TRAP::Graphics::API::OpenGLTextureCube::SplitImageFromCross(const Scope<Image>& image,
                                                                                          const InputFormat format,
																						  const uint32_t faceWidth,
																						  const uint32_t faceHeight,
																						  uint32_t& face,
																						  const uint32_t stride)
{
	uint32_t cxLimit = 3, cyLimit = 4;
	if (format == InputFormat::Horizontal_Cross)
	{
		cxLimit = 4;
		cyLimit = 3;
	}
	
	std::array<std::vector<T>, 6> cubeTextureData;
	for (auto& i : cubeTextureData)
		i.resize(faceWidth * faceHeight * stride);

	for (uint32_t cy = 0; cy < cyLimit; cy++)
	{
		for (uint32_t cx = 0; cx < cxLimit; cx++)
		{
			if (cy == 0 || cy == 2 || cy == 3)
			{
				if (cx != 1)
					continue;
			}

			for (uint32_t y = 0; y < faceHeight; y++)
			{
				uint32_t offset = y;
				if(format == InputFormat::Vertical_Cross && face == 5)
					offset = faceHeight - (y + 1);
				const uint32_t yp = cy * faceHeight + offset;
				
				for (uint32_t x = 0; x < faceWidth; x++)
				{
					offset = x;
					if (format == InputFormat::Vertical_Cross && face == 5)
						offset = faceWidth - (x + 1);
					const uint32_t xp = cx * faceWidth + offset;
					switch (stride)
					{
					case 1:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 0];
						break;

					case 2:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 1];
						break;

					case 3:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 1];
						cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 2];
						break;

					case 4:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 1];
						cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 2];
						cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<const T*>(image->GetPixelData())[(xp + yp * image->GetWidth()) * stride + 3];
						break;

					default:
						break;
					}
				}
			}
			face++;
		}
	}
	
	//Now swap images into correct order for OpenGL
	std::swap(cubeTextureData[3], cubeTextureData[0]);
	std::swap(cubeTextureData[3], cubeTextureData[2]);
	if(format == InputFormat::Horizontal_Cross)
	{
		std::swap(cubeTextureData[3], cubeTextureData[5]);
		std::swap(cubeTextureData[5], cubeTextureData[4]);
	}
	else
		std::swap(cubeTextureData[3], cubeTextureData[4]);

	return cubeTextureData;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::UploadImage(const Scope<Image>& image, const uint32_t faceWidth,
	const uint32_t faceHeight, uint32_t& face, const uint32_t stride) const
{
	if (image->IsHDR())
	{
		std::array<std::vector<float>, 6> cubeTextureData = SplitImageFromCross<float>(image, InputFormat::Vertical_Cross, faceWidth, faceHeight, face, stride);

		for (uint32_t i = 0; i < 6; i++)
			glTextureSubImage3D(m_handle, 0, 0, 0, i, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_FLOAT, cubeTextureData[i].data());
	}
	else if ((image->IsImageGrayScale() && image->GetBitsPerPixel() == 16 && !image->HasAlphaChannel()) ||
		(image->IsImageGrayScale() && image->GetBitsPerPixel() == 32 && image->HasAlphaChannel()) ||
		(image->IsImageColored() && image->GetBitsPerPixel() == 48 && !image->HasAlphaChannel()) ||
		(image->IsImageColored() && image->GetBitsPerPixel() == 64 && image->HasAlphaChannel()))
	{
		std::array<std::vector<uint16_t>, 6> cubeTextureData = SplitImageFromCross<uint16_t>(image, InputFormat::Vertical_Cross, faceWidth, faceHeight, face, stride);

		for (uint32_t i = 0; i < 6; i++)
			glTextureSubImage3D(m_handle, 0, 0, 0, i, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[i].data());
	}
	else
	{
		std::array<std::vector<uint8_t>, 6> cubeTextureData = SplitImageFromCross<uint8_t>(image, InputFormat::Vertical_Cross, faceWidth, faceHeight, face, stride);

		for (uint32_t i = 0; i < 6; i++)
			glTextureSubImage3D(m_handle, 0, 0, 0, i, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[i].data());
	}
}
