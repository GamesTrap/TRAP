#include "TRAPPCH.h"
#include "OpenGLTextureCube.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
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
	{
		OpenGLCall(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize)));
	}
	Scope<Image> image = Image::LoadFallback();
	image = Image::FlipY(image);

	InitializeTexture();

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(image->GetWidth(), image->GetHeight())))); //Same as specification
	OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(image->GetColorFormat(), image->GetBytesPerPixel()), image->GetWidth(), image->GetHeight()));
	
	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, image->GetWidth(), image->GetHeight(), 1, TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_UNSIGNED_BYTE,
		image->GetPixelData()));
	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, image->GetWidth(), image->GetHeight(), 1, TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_UNSIGNED_BYTE,
		image->GetPixelData()));

	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, image->GetWidth(), image->GetHeight(), 1, TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_UNSIGNED_BYTE,
		image->GetPixelData()));
	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, image->GetWidth(), image->GetHeight(), 1, TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_UNSIGNED_BYTE,
		image->GetPixelData()));

	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, image->GetWidth(), image->GetHeight(), 1, TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_UNSIGNED_BYTE,
		image->GetPixelData()));
	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, image->GetWidth(), image->GetHeight(), 1, TRAPImageFormatToOpenGL(image->GetColorFormat()), GL_UNSIGNED_BYTE,
		image->GetPixelData()));

	OpenGLCall(glGenerateTextureMipmap(m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::OpenGLTextureCube(std::string name, const std::array<std::string, 6>& filepaths, const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepaths =
	{
		VFS::MakeVirtualPathCompatible(filepaths[0]),
		VFS::MakeVirtualPathCompatible(filepaths[1]),
		VFS::MakeVirtualPathCompatible(filepaths[2]),
		VFS::MakeVirtualPathCompatible(filepaths[3]),
		VFS::MakeVirtualPathCompatible(filepaths[4]),
		VFS::MakeVirtualPathCompatible(filepaths[5])
	};
	m_textureParameters = parameters;
	m_inputFormat = InputFormat::NONE;
	
	TP_DEBUG(Log::TextureCubeOpenGLPrefix, "Loading Texture: \"", m_name, "\"");
	if (s_maxCubeTextureSize == 0) //Only load maximum available texture size once
	{
		OpenGLCall(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize)));
	}

	for (const auto& path : m_filepaths)
		if (path.empty())
		{
			TP_ERROR(Log::TextureCubeOpenGLPrefix, "Missing File(s)!");
			TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
			return;
		}

	LoadFiles();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::OpenGLTextureCube(std::string name, const std::string& filepath, const InputFormat format, const TextureParameters parameters)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepaths[0] = VFS::MakeVirtualPathCompatible(filepath);
	m_textureParameters = parameters;
	m_inputFormat = format;
	
	TP_DEBUG(Log::TextureCubeOpenGLPrefix, "Loading Texture: \"", m_name, "\"");
	if (s_maxCubeTextureSize == 0) //Only load maximum available texture size once
	{
		OpenGLCall(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize)));
	}

	if (m_filepaths[0].empty())
	{
		TP_ERROR(Log::TextureCubeOpenGLPrefix, "Filepath is empty!");
		TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
		return;
	}

	if (m_inputFormat == InputFormat::Vertical_Cross)
		SetupVerticalCross();
	else if (m_inputFormat == InputFormat::Horizontal_Cross)
		SetupHorizontalCross();
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
		m_filepaths[0] = VFS::MakeVirtualPathCompatible(std::string(img->GetFilePath()));

		TP_DEBUG(Log::TextureCubeOpenGLPrefix, "Loading Texture: \"", m_name, "\"");
		if (s_maxCubeTextureSize == 0) //Only load maximum available texture size once
		{
			OpenGLCall(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize)));
		}

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

			LoadVerticalCross(img);
		}
		else
		{
			if (img->GetWidth() <= img->GetHeight())
			{
				TP_ERROR(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Invalid InputFormat usage!");
				TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
				return;
			}

			LoadHorizontalCross(img);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::~OpenGLTextureCube()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG(Log::TextureCubeOpenGLPrefix, "Destroying TextureCube: \"", m_name, "\"");
	
	if(m_handle)
	{
		OpenGLCall(glDeleteTextures(1, &m_handle));		
	}
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
			{
				OpenGLCall(glBindTextureUnit(slot, m_handle));
			}
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
		OpenGLCall(glBindTextureUnit(slot, 0));
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
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(wrap)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::SetFilter(const TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Filter = filter;
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::SetupVerticalCross()
{
	std::filesystem::path physicalPath;
	if (!VFS::SilentResolveReadPhysicalPath(m_filepaths[0], physicalPath))
	{
		TP_ERROR(Log::TextureCubeOpenGLPrefix, "Couldn't resolve FilePath: ", m_filepaths[0], "!");
		TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map!");
		return;
	}

	const Scope<Image> image = Image::LoadFromFile(m_filepaths[0]);

	if (!CheckImageSize(image))
		return;
	if (image->GetHeight() <= image->GetWidth())
	{
		TP_ERROR(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Invalid InputFormat usage!");
		TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
		return;
	}

	LoadVerticalCross(image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::SetupHorizontalCross()
{
	std::filesystem::path physicalPath;
	if (!VFS::SilentResolveReadPhysicalPath(m_filepaths[0], physicalPath))
	{
		TP_ERROR(Log::TextureCubeOpenGLPrefix, "Couldn't resolve FilePath: ", m_filepaths[0], "!");
		TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map!");
		return;
	}

	const Scope<Image> image = Image::LoadFromFile(m_filepaths[0]);

	if (!CheckImageSize(image))
		return;
	if (image->GetWidth() <= image->GetHeight())
	{
		TP_ERROR(Log::TextureCubeOpenGLPrefix, "Texture: \"", m_name, "\" Invalid InputFormat usage!");
		TP_WARN(Log::TextureCubeOpenGLPrefix, "Using Default Cube Map");
		return;
	}

	LoadHorizontalCross(image);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::LoadVerticalCross(const Scope<Image>& img)
{
	const uint32_t stride = img->GetBitsPerPixel() / 8;
	uint32_t face = 0;
	const uint32_t faceWidth = img->GetWidth() / 3;
	const uint32_t faceHeight = img->GetHeight() / 4;

	bool resetPixelStore = false;
	InitializeTexture();

	//if (!std::ispow2(faceWidth) || !std::ispow2(faceHeight))
	if (!Math::IsPowerOfTwo(faceWidth) || !Math::IsPowerOfTwo(faceHeight))
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(faceWidth, faceHeight)))); //Same as specification
	OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(img->GetColorFormat(), img->GetBytesPerPixel()), faceWidth, faceHeight));

	if (img->IsHDR())
	{
		std::array<std::vector<float>, 6> cubeTextureData;
		for (auto& i : cubeTextureData)
			i.resize(faceWidth * faceHeight * stride);

		for (uint32_t cy = 0; cy < 4; cy++)
		{
			for (uint32_t cx = 0; cx < 3; cx++)
			{
				if (cy == 0 || cy == 2 || cy == 3)
				{
					if (cx != 1)
						continue;
				}

				for (uint32_t y = 0; y < faceHeight; y++)
				{
					uint32_t offset = y;
					if (face == 5)
						offset = faceHeight - (y + 1);
					const uint32_t yp = cy * faceHeight + offset;
					for (uint32_t x = 0; x < faceWidth; x++)
					{
						offset = x;
						if (face == 5)
							offset = faceWidth - (x + 1);
						const uint32_t xp = cx * faceWidth + offset;
						switch (stride)
						{
						case 1:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[5].data()));
	}
	else if ((img->IsImageGrayScale() && img->GetBitsPerPixel() == 16 && !img->HasAlphaChannel()) ||
		(img->IsImageGrayScale() && img->GetBitsPerPixel() == 32 && img->HasAlphaChannel()) ||
		(img->IsImageColored() && img->GetBitsPerPixel() == 48 && !img->HasAlphaChannel()) ||
		(img->IsImageColored() && img->GetBitsPerPixel() == 64 && img->HasAlphaChannel()))
	{
		std::vector<std::vector<uint16_t>> cubeTextureData;
		cubeTextureData.resize(6);
		for (auto& i : cubeTextureData)
			i.resize(faceWidth * faceHeight * stride);

		for (uint32_t cy = 0; cy < 4; cy++)
		{
			for (uint32_t cx = 0; cx < 3; cx++)
			{
				if (cy == 0 || cy == 2 || cy == 3)
				{
					if (cx != 1)
						continue;
				}

				for (uint32_t y = 0; y < faceHeight; y++)
				{
					uint32_t offset = y;
					if (face == 5)
						offset = faceHeight - (y + 1);
					const uint32_t yp = cy * faceHeight + offset;
					for (uint32_t x = 0; x < faceWidth; x++)
					{
						offset = x;
						if (face == 5)
							offset = faceWidth - (x + 1);
						const uint32_t xp = cx * faceWidth + offset;
						switch (stride)
						{
						case 1:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[5].data()));
	}
	else
	{
		std::vector<std::vector<uint8_t>> cubeTextureData;
		cubeTextureData.resize(6);
		for (auto& i : cubeTextureData)
			i.resize(faceWidth * faceHeight * stride);

		for (uint32_t cy = 0; cy < 4; cy++)
		{
			for (uint32_t cx = 0; cx < 3; cx++)
			{
				if (cy == 0 || cy == 2 || cy == 3)
				{
					if (cx != 1)
						continue;
				}

				for (uint32_t y = 0; y < faceHeight; y++)
				{
					uint32_t offset = y;
					if (face == 5)
						offset = faceHeight - (y + 1);
					const uint32_t yp = cy * faceHeight + offset;
					for (uint32_t x = 0; x < faceWidth; x++)
					{
						offset = x;
						if (face == 5)
							offset = faceWidth - (x + 1);
						const uint32_t xp = cx * faceWidth + offset;
						switch (stride)
						{
						case 1:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[5].data()));
	}

	if (img->HasAlphaChannel() && img->IsImageGrayScale())
	{
		OpenGLCall(glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_GREEN}.data()));
	}
	else if (img->IsImageGrayScale())
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

void TRAP::Graphics::API::OpenGLTextureCube::LoadHorizontalCross(const Scope<Image>& img)
{
	const uint32_t stride = img->GetBitsPerPixel() / 8;
	uint32_t face = 0;
	const uint32_t faceWidth = img->GetWidth() / 4;
	const uint32_t faceHeight = img->GetHeight() / 3;

	bool resetPixelStore = false;
	InitializeTexture();

	//if (!std::ispow2(faceWidth) || !std::ispow2(faceHeight))
	if (!Math::IsPowerOfTwo(faceWidth) || !Math::IsPowerOfTwo(faceHeight))
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(faceWidth, faceHeight)))); //Same as specification
	OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(img->GetColorFormat(), img->GetBytesPerPixel()), faceWidth, faceHeight));

	if (img->IsHDR())
	{
		std::array<std::vector<float>, 6> cubeTextureData;
		for (auto& i : cubeTextureData)
			i.resize(faceWidth * faceHeight * stride);

		for (uint32_t cy = 0; cy < 3; cy++)
		{
			for (uint32_t cx = 0; cx < 4; cx++)
			{
				if (cy == 0 || cy == 2 || cy == 3)
				{
					if (cx != 1)
						continue;
				}

				for (uint32_t y = 0; y < faceHeight; y++)
				{
					uint32_t offset = y;
					const uint32_t yp = cy * faceHeight + offset;
					for (uint32_t x = 0; x < faceWidth; x++)
					{
						offset = x;
						const uint32_t xp = cx * faceWidth + offset;
						switch (stride)
						{
						case 1:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<const float*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_FLOAT, cubeTextureData[5].data()));
	}
	else if ((img->IsImageGrayScale() && img->GetBitsPerPixel() == 16 && !img->HasAlphaChannel()) ||
		(img->IsImageGrayScale() && img->GetBitsPerPixel() == 32 && img->HasAlphaChannel()) ||
		(img->IsImageColored() && img->GetBitsPerPixel() == 48 && !img->HasAlphaChannel()) ||
		(img->IsImageColored() && img->GetBitsPerPixel() == 64 && img->HasAlphaChannel()))
	{
		std::vector<std::vector<uint16_t>> cubeTextureData;
		cubeTextureData.resize(6);
		for (auto& i : cubeTextureData)
			i.resize(faceWidth * faceHeight * stride);

		for (uint32_t cy = 0; cy < 3; cy++)
		{
			for (uint32_t cx = 0; cx < 4; cx++)
			{
				if (cy == 0 || cy == 2 || cy == 3)
				{
					if (cx != 1)
						continue;
				}

				for (uint32_t y = 0; y < faceHeight; y++)
				{
					uint32_t offset = y;
					const uint32_t yp = cy * faceHeight + offset;
					for (uint32_t x = 0; x < faceWidth; x++)
					{
						offset = x;
						const uint32_t xp = cx * faceWidth + offset;
						switch (stride)
						{
						case 1:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<const uint16_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_SHORT, cubeTextureData[5].data()));
	}
	else
	{
		std::vector<std::vector<uint8_t>> cubeTextureData;
		cubeTextureData.resize(6);
		for (auto& i : cubeTextureData)
			i.resize(faceWidth * faceHeight * stride);

		for (uint32_t cy = 0; cy < 3; cy++)
		{
			for (uint32_t cx = 0; cx < 4; cx++)
			{
				if (cy == 0 || cy == 2 || cy == 3)
				{
					if (cx != 1)
						continue;
				}

				for (uint32_t y = 0; y < faceHeight; y++)
				{
					uint32_t offset = y;
					const uint32_t yp = cy * faceHeight + offset;
					for (uint32_t x = 0; x < faceWidth; x++)
					{
						offset = x;
						const uint32_t xp = cx * faceWidth + offset;
						switch (stride)
						{
						case 1:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							break;
							
						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<const uint8_t*>(img->GetPixelData())[(xp + yp * img->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[5].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(img->GetColorFormat()), GL_UNSIGNED_BYTE, cubeTextureData[4].data()));

		if (img->HasAlphaChannel() && img->IsImageGrayScale())
		{
			OpenGLCall(glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_GREEN}.data()));
		}
		else if (img->IsImageGrayScale())
		{
			OpenGLCall(glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_ONE}.data()));
		}
	}

	if (resetPixelStore)
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
	}

	OpenGLCall(glGenerateTextureMipmap(m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::LoadFiles()
{
	std::array<Scope<Image>, 6> images{};
	for (uint32_t i = 0; i < m_filepaths.size(); i++)
	{
		std::filesystem::path physicalPath;
		if (!VFS::SilentResolveReadPhysicalPath(m_filepaths[i], physicalPath))
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
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(width, height)))); //Same as specification
	OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(images[0]->GetColorFormat(), images[0]->GetBytesPerPixel()), width, height));

	for (uint32_t i = 0; i < 6; i++)
	{
		if (images[i]->IsHDR())
		{
			OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, i, width, height, 1, TRAPImageFormatToOpenGL(images[0]->GetColorFormat()), GL_FLOAT, images[i]->GetPixelData()));
		}
		else if ((images[i]->IsImageGrayScale() && images[i]->GetBitsPerPixel() == 16 && !images[i]->HasAlphaChannel()) ||
			(images[i]->IsImageGrayScale() && images[i]->GetBitsPerPixel() == 32 && images[i]->HasAlphaChannel()) ||
			(images[i]->IsImageColored() && images[i]->GetBitsPerPixel() == 48 && !images[i]->HasAlphaChannel()) ||
			(images[i]->IsImageColored() && images[i]->GetBitsPerPixel() == 64 && images[i]->HasAlphaChannel()))
		{
			OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, i, width, height, 1, TRAPImageFormatToOpenGL(images[i]->GetColorFormat()), GL_UNSIGNED_SHORT, images[i]->GetPixelData()));
		}
		else
		{
			OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, i, width, height, 1, TRAPImageFormatToOpenGL(images[i]->GetColorFormat()), GL_UNSIGNED_BYTE, images[i]->GetPixelData()));
		}
	}

	if (hasAlphaChannel && isGrayScale)
	{
		OpenGLCall(glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_GREEN}.data()));
	}
	else if (isGrayScale)
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
	OpenGLCall(glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_handle));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_textureParameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_textureParameters.Wrap)));
}