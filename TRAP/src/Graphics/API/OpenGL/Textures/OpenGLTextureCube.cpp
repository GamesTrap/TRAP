#include "TRAPPCH.h"
#include "OpenGLTextureCube.h"

#include "Graphics/API/OpenGL/OpenGLCommon.h"
#include "Graphics/Textures/TextureManager.h"
#include "VFS/VFS.h"

TRAP::Graphics::API::OpenGLTextureCube::OpenGLTextureCube(const TextureParameters parameters)
	: m_handle(0), m_name("FallbackCube"), m_parameters(parameters), m_inputFormat(InputFormat::NONE)
{
	TP_DEBUG("[TextureCube][OpenGL] Loading Texture: \"", m_name, "\"");
	m_images[0] = Image::LoadFallback();

	OpenGLCall(glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_handle));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(m_images[0]->GetWidth(), m_images[0]->GetHeight())))); //Same as specification
	OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(m_images[0]->GetFormat(), m_images[0]->GetBytesPerPixel()), m_images[0]->GetWidth(), m_images[0]->GetHeight()));

	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, m_images[0]->GetWidth(), m_images[0]->GetHeight(), 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE,
		TRAP::Image::FlipY(m_images[0]->GetWidth(), m_images[0]->GetHeight(), m_images[0]->GetFormat(), static_cast<uint8_t*>(m_images[0]->GetPixelData())).data()));
	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, m_images[0]->GetWidth(), m_images[0]->GetHeight(), 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE,
		TRAP::Image::FlipY(m_images[0]->GetWidth(), m_images[0]->GetHeight(), m_images[0]->GetFormat(), static_cast<uint8_t*>(m_images[0]->GetPixelData())).data()));

	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, m_images[0]->GetWidth(), m_images[0]->GetHeight(), 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE,
		TRAP::Image::FlipY(m_images[0]->GetWidth(), m_images[0]->GetHeight(), m_images[0]->GetFormat(), static_cast<uint8_t*>(m_images[0]->GetPixelData())).data()));
	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, m_images[0]->GetWidth(), m_images[0]->GetHeight(), 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE,
		TRAP::Image::FlipY(m_images[0]->GetWidth(), m_images[0]->GetHeight(), m_images[0]->GetFormat(), static_cast<uint8_t*>(m_images[0]->GetPixelData())).data()));

	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, m_images[0]->GetWidth(), m_images[0]->GetHeight(), 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE,
		TRAP::Image::FlipY(m_images[0]->GetWidth(), m_images[0]->GetHeight(), m_images[0]->GetFormat(), static_cast<uint8_t*>(m_images[0]->GetPixelData())).data()));
	OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, m_images[0]->GetWidth(), m_images[0]->GetHeight(), 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE,
		TRAP::Image::FlipY(m_images[0]->GetWidth(), m_images[0]->GetHeight(), m_images[0]->GetFormat(), static_cast<uint8_t*>(m_images[0]->GetPixelData())).data()));

	OpenGLCall(glGenerateTextureMipmap(m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::OpenGLTextureCube(std::string name, const std::array<std::string, 6> & filepaths, const TextureParameters parameters)
	: m_handle(0), m_name(std::move(name)), m_parameters(parameters), m_inputFormat(InputFormat::NONE)
{
	TP_DEBUG("[TextureCube][OpenGL] Loading Texture: \"", m_name, "\"");
	if (s_maxCubeTextureSize == 0) //Only load maximum available texture size once
	{
		OpenGLCall(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize)));
	}

	for (const auto& path : filepaths)
		if (path.empty())
		{
			TP_ERROR("[TextureCube][OpenGL] FilePaths are empty!");
			TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
			return;
		}

	LoadFiles(filepaths);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::OpenGLTextureCube(std::string name, const std::string& filepath, const InputFormat format, const TextureParameters parameters)
	: m_handle(0), m_name(std::move(name)), m_parameters(parameters), m_inputFormat(format)
{
	TP_DEBUG("[TextureCube][OpenGL] Loading Texture: \"", m_name, "\"");
	if (s_maxCubeTextureSize == 0) //Only load maximum available texture size once
	{
		OpenGLCall(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, reinterpret_cast<int32_t*>(&s_maxCubeTextureSize)));
	}

	if (filepath.empty())
	{
		TP_ERROR("[TextureCube][OpenGL] Filepath is empty!");
		TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
		return;
	}

	if (m_inputFormat == InputFormat::Vertical_Cross)
		LoadVerticalCross(filepath);
	else if (m_inputFormat == InputFormat::Horizontal_Cross)
		LoadHorizontalCross(filepath);
	else if (m_inputFormat == InputFormat::NONE)
	{
		TP_ERROR("[TextureCube][OpenGL] InputFormat is None!");
		TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
		return;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLTextureCube::~OpenGLTextureCube()
{
	TP_DEBUG("[TextureCube][OpenGL] Destroying TextureCube: \"", m_name, "\"");
	OpenGLCall(glDeleteTextures(1, &m_handle));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::Bind(const uint32_t slot) const
{
	if (m_handle)
	{
		OpenGLCall(glBindTextureUnit(slot, m_handle));
	}
	else
		TextureManager::Get("FallbackCube", TextureType::TextureCube)->Bind();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::Unbind(uint32_t slot) const
{
	OpenGLCall(glBindTextureUnit(0, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::SetFilter(const TextureFilter filter)
{
	m_parameters.Filter = filter;
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::SetWrap(const TextureWrap wrap)
{
	m_parameters.Wrap = wrap;
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, TRAPTextureWrapToOpenGL(wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(wrap)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLTextureCube::LoadVerticalCross(const std::string& filepath)
{
	const std::string virtualFilePath = VFS::MakeVirtualPathCompatible(filepath);
	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(virtualFilePath, physicalPath))
	{
		TP_ERROR("[TextureCube][OpenGL] Couldn't resolve FilePath: ", virtualFilePath, "!");
		TP_WARN("[TextureCube][OpenGL] Using Default Cube Map!");
		return;
	}

	m_images[0] = Image::LoadFromFile(filepath);

	if (m_images[0]->GetWidth() > s_maxCubeTextureSize || m_images[0]->GetHeight() > s_maxCubeTextureSize)
	{
		TP_CRITICAL("[TextureCube][OpenGL] Texture: \"", m_name, "\" Width: ", m_images[0]->GetWidth(), " or Height: ", m_images[0]->GetHeight(), " is bigger than the maximum allowed texture size(", s_maxCubeTextureSize, ")!");
		TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
		return;
	}
	if (m_images[0]->GetHeight() <= m_images[0]->GetWidth())
	{
		TP_ERROR("[TextureCube][OpenGL] Texture: \"", m_name, "\" Invalid InputFormat usage!");
		TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
		return;
	}

	const uint32_t stride = m_images[0]->GetBitsPerPixel() / 8;
	uint32_t face = 0;
	const uint32_t faceWidth = m_images[0]->GetWidth() / 3;
	const uint32_t faceHeight = m_images[0]->GetHeight() / 4;

	bool resetPixelStore = false;
	OpenGLCall(glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_handle));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));

	//if (!std::ispow2(faceWidth) || !std::ispow2(faceHeight))
	if (!Math::IsPow2(faceWidth) || !Math::IsPow2(faceHeight))
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(faceWidth, faceHeight)))); //Same as specification
	OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(m_images[0]->GetFormat(), m_images[0]->GetBytesPerPixel()), faceWidth, faceHeight));

	if (m_images[0]->IsHDR())
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
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[5].data()));
	}
	else if ((m_images[0]->IsImageGrayScale() && m_images[0]->GetBitsPerPixel() == 16 && !m_images[0]->HasAlphaChannel()) ||
		(m_images[0]->IsImageGrayScale() && m_images[0]->GetBitsPerPixel() == 24 && m_images[0]->HasAlphaChannel()) ||
		(m_images[0]->IsImageColored() && m_images[0]->GetBitsPerPixel() == 48 && !m_images[0]->HasAlphaChannel()) ||
		(m_images[0]->IsImageColored() && m_images[0]->GetBitsPerPixel() == 64 && m_images[0]->HasAlphaChannel()))
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
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[5].data()));
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
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[5].data()));
	}

	if (m_images[0]->HasAlphaChannel() && m_images[0]->IsImageGrayScale())
	{
		OpenGLCall(glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_GREEN}.data()));
	}
	else if (m_images[0]->IsImageGrayScale())
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

void TRAP::Graphics::API::OpenGLTextureCube::LoadHorizontalCross(const std::string& filepath)
{
	const std::string virtualFilePath = VFS::MakeVirtualPathCompatible(filepath);
	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(virtualFilePath, physicalPath))
	{
		TP_ERROR("[TextureCube][OpenGL] Couldn't resolve FilePath: ", virtualFilePath, "!");
		TP_WARN("[TextureCube][OpenGL] Using Default Cube Map!");
		return;
	}

	m_images[0] = Image::LoadFromFile(filepath);

	if (m_images[0]->GetWidth() > s_maxCubeTextureSize || m_images[0]->GetHeight() > s_maxCubeTextureSize)
	{
		TP_CRITICAL("[TextureCube][OpenGL] Texture: \"", m_name, "\" Width: ", m_images[0]->GetWidth(), " or Height: ", m_images[0]->GetHeight(), " is bigger than the maximum allowed texture size(", s_maxCubeTextureSize, ")!");
		TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
		return;
	}
	if (m_images[0]->GetWidth() <= m_images[0]->GetHeight())
	{
		TP_ERROR("[TextureCube][OpenGL] Texture: \"", m_name, "\" Invalid InputFormat usage!");
		TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
		return;
	}

	const uint32_t stride = m_images[0]->GetBitsPerPixel() / 8;
	uint32_t face = 0;
	const uint32_t faceWidth = m_images[0]->GetWidth() / 4;
	const uint32_t faceHeight = m_images[0]->GetHeight() / 3;

	bool resetPixelStore = false;
	OpenGLCall(glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_handle));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));

	//if (!std::ispow2(faceWidth) || !std::ispow2(faceHeight))
	if (!Math::IsPow2(faceWidth) || !Math::IsPow2(faceHeight))
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(faceWidth, faceHeight)))); //Same as specification
	OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(m_images[0]->GetFormat(), m_images[0]->GetBytesPerPixel()), faceWidth, faceHeight));

	if (m_images[0]->IsHDR())
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
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<float*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, cubeTextureData[5].data()));
	}
	else if ((m_images[0]->IsImageGrayScale() && m_images[0]->GetBitsPerPixel() == 16 && !m_images[0]->HasAlphaChannel()) ||
		(m_images[0]->IsImageGrayScale() && m_images[0]->GetBitsPerPixel() == 24 && m_images[0]->HasAlphaChannel()) ||
		(m_images[0]->IsImageColored() && m_images[0]->GetBitsPerPixel() == 48 && !m_images[0]->HasAlphaChannel()) ||
		(m_images[0]->IsImageColored() && m_images[0]->GetBitsPerPixel() == 64 && m_images[0]->HasAlphaChannel()))
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
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<uint16_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[4].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_SHORT, cubeTextureData[5].data()));
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
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							break;

						case 2:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							break;

						case 3:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							break;

						case 4:
							cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 0];
							cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 1];
							cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 2];
							cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<uint8_t*>(m_images[0]->GetPixelData())[(xp + yp * m_images[0]->GetWidth()) * stride + 3];
							break;

						default:
							break;
						}
					}
				}
				face++;
			}
		}

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 0, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[3].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 1, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[1].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 2, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[0].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 3, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[5].data()));

		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 4, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[2].data()));
		OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, 5, faceWidth, faceHeight, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_UNSIGNED_BYTE, cubeTextureData[4].data()));

		if (m_images[0]->HasAlphaChannel() && m_images[0]->IsImageGrayScale())
		{
			OpenGLCall(glTextureParameteriv(m_handle, GL_TEXTURE_SWIZZLE_RGBA, std::array<int32_t, 4>{GL_RED, GL_RED, GL_RED, GL_GREEN}.data()));
		}
		else if (m_images[0]->IsImageGrayScale())
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

void TRAP::Graphics::API::OpenGLTextureCube::LoadFiles(const std::array<std::string, 6> & filepaths)
{
	for (uint32_t i = 0; i < filepaths.size(); i++)
	{
		const std::string virtualFilePath = VFS::MakeVirtualPathCompatible(filepaths[i]);
		std::filesystem::path physicalPath;
		if (!VFS::Get()->SilentResolveReadPhysicalPath(virtualFilePath, physicalPath))
		{
			TP_ERROR("[TextureCube][OpenGL] Couldn't resolve FilePath: ", virtualFilePath, "!");
			TP_WARN("[TextureCube][OpenGL] Using Default Cube Map!");
			return;
		}

		m_images[i] = Image::LoadFromFile(filepaths[i]);
	}

	const uint32_t width = m_images[0]->GetWidth();
	const uint32_t height = m_images[0]->GetHeight();
	const bool isGrayScale = m_images[0]->IsImageGrayScale();
	const bool hasAlphaChannel = m_images[0]->HasAlphaChannel();

	for (const auto& image : m_images)
	{
		if (image->GetWidth() > s_maxCubeTextureSize || image->GetHeight() > s_maxCubeTextureSize)
		{
			TP_CRITICAL("[TextureCube][OpenGL] Texture: \"", m_name, "\" Width: ", image->GetWidth(), " or Height: ", image->GetHeight(), " is bigger than the maximum allowed texture size(", s_maxCubeTextureSize, ")!");
			TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
			return;
		}

		if (image->GetWidth() != width || image->GetHeight() != height)
		{
			TP_ERROR("[TextureCube][OpenGL] Texture: \"", m_name, "\" Image size: ", image->GetWidth(), "x", image->GetHeight(), " is not the same size as the other images!");
			TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
			return;
		}

		if(image->IsImageGrayScale() != isGrayScale && image->HasAlphaChannel() != hasAlphaChannel)
		{
			TP_ERROR("[TextureCube][OpenGL] Texture: \"", m_name, "\" Image: \"", image->GetFilePath(), "\" doesn't match other images properties!");
			TP_WARN("[TextureCube][OpenGL] Using Default Cube Map");
			return;
		}
	}

	bool resetPixelStore = false;
	OpenGLCall(glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_handle));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_parameters.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));
	OpenGLCall(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, TRAPTextureWrapToOpenGL(m_parameters.Wrap)));

	//if (!std::ispow2(faceWidth) || !std::ispow2(faceHeight))
	if (!Math::IsPow2(width) || !Math::IsPow2(height))
	{
		OpenGLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		resetPixelStore = true;
	}

	const uint32_t numMipMapLevels = 1 + static_cast<uint32_t>(std::floor(std::log2(Math::Max(width, height)))); //Same as specification
	OpenGLCall(glTextureStorage2D(m_handle, numMipMapLevels, TRAPImageFormatToOpenGLPrecise(m_images[0]->GetFormat(), m_images[0]->GetBytesPerPixel()), width, height));

	for (uint32_t i = 0; i < 6; i++)
	{
		if (m_images[i]->IsHDR())
		{
			OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, i, width, height, 1, TRAPImageFormatToOpenGL(m_images[0]->GetFormat()), GL_FLOAT, m_images[i]->GetPixelData()));
		}
		else if ((m_images[i]->IsImageGrayScale() && m_images[i]->GetBitsPerPixel() == 16 && !m_images[i]->HasAlphaChannel()) ||
			(m_images[i]->IsImageGrayScale() && m_images[i]->GetBitsPerPixel() == 24 && m_images[i]->HasAlphaChannel()) ||
			(m_images[i]->IsImageColored() && m_images[i]->GetBitsPerPixel() == 48 && !m_images[i]->HasAlphaChannel()) ||
			(m_images[i]->IsImageColored() && m_images[i]->GetBitsPerPixel() == 64 && m_images[i]->HasAlphaChannel()))
		{
			OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, i, width, height, 1, TRAPImageFormatToOpenGL(m_images[i]->GetFormat()), GL_UNSIGNED_SHORT, m_images[i]->GetPixelData()));
		}
		else
		{
			OpenGLCall(glTextureSubImage3D(m_handle, 0, 0, 0, i, width, height, 1, TRAPImageFormatToOpenGL(m_images[i]->GetFormat()), GL_UNSIGNED_BYTE, m_images[i]->GetPixelData()));
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