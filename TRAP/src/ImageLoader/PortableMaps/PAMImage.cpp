#include "TRAPPCH.h"
#include "PAMImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"

TRAP::INTERNAL::PAMImage::PAMImage(std::string filepath)
	: m_filepath(std::move(filepath)), m_bitsPerPixel(0), m_isImageGrayScale(false), m_isImageColored(false), m_hasAlphaChannel(false), m_width(0), m_height(0), m_format(ImageFormat::NONE)
{
	TP_DEBUG("[Image][PAM] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TP_ERROR("[Image][PAM] Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN("[Image][PAM] Using Default Image!");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR("[Image][PAM] Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN("[Image][PAM] Using Default Image!");
			return;
		}

		std::string tempTrash;
		struct Header
		{
			std::string MagicNumber = "";
			std::uint32_t Width = 0;
			std::uint32_t Height = 0;
			std::uint32_t Depth = 0;
			std::uint32_t MaxValue = 255;
			std::string TuplType = "";
		} header;
		file >> header.MagicNumber >> tempTrash >> header.Width >> tempTrash >> header.Height >> tempTrash >> header.Depth >> tempTrash >> header.MaxValue >> tempTrash >> header.TuplType >> tempTrash;
		file.ignore(256, '\n'); //Skip ahead to the pixel data.

		if (header.MagicNumber != "P7")
		{
			file.close();
			TP_ERROR("[Image][PAM] Invalid Magic Number!");
			TP_WARN("[Image][PAM] Using Default Image!");
			return;
		}
		if (header.Width < 1)
		{
			file.close();
			TP_ERROR("[Image][PAM] Width is < 1!");
			TP_WARN("[Image][PAM] Using Default Image!");
			return;
		}
		if (header.Height < 1)
		{
			file.close();
			TP_ERROR("[Image][PAM] Height is < 1!");
			TP_WARN("[Image][PAM] Using Default Image!");
			return;
		}
		if (header.MaxValue < 1 || header.MaxValue > 65535)
		{
			file.close();
			TP_ERROR("[Image][PAM] MaxValue is unsupported/invalid!");
			TP_WARN("[Image][PAM] Using Default Image!");
			return;
		}
		if (!(header.TuplType == "GRAYSCALE" || header.TuplType == "RGB" || header.TuplType == "GRAYSCALE_ALPHA" || header.TuplType == "RGB_ALPHA"))
		{
			file.close();
			TP_ERROR("[Image][PAM] TuplType is unsupported or invalid!");
			TP_WARN("[Image][PAM] Using Default Image!");
			return;
		}

		m_width = header.Width;
		m_height = header.Height;

		if(header.MaxValue > 255)
		{
			if (header.TuplType == "GRAYSCALE" && header.Depth == 1)
			{
				//GrayScale
				m_isImageGrayScale = true;
				m_bitsPerPixel = 16;
				m_format = ImageFormat::Gray_Scale;
			}
			else if (header.TuplType == "RGB" && header.Depth == 3)
			{
				//RGB
				m_isImageColored = true;
				m_bitsPerPixel = 48;
				m_format = ImageFormat::RGB;
			}
			else if (header.TuplType == "GRAYSCALE_ALPHA" && header.Depth == 2)
			{
				//GrayScaleAlpha
				m_isImageGrayScale = true;
				m_hasAlphaChannel = true;
				m_bitsPerPixel = 32;
				m_format = ImageFormat::Gray_Scale_Alpha;
			}
			else if (header.TuplType == "RGB_ALPHA" && header.Depth == 4)
			{
				//RGBA
				m_isImageColored = true;
				m_hasAlphaChannel = true;
				m_bitsPerPixel = 64;
				m_format = ImageFormat::RGBA;
			}

			m_data.reserve(m_width * m_height * header.Depth);
			short temp;
			for (unsigned int i = 0; i < m_width * m_height * header.Depth; i++)
			{
				file.read(reinterpret_cast<char*>(&temp), sizeof(uint16_t));
				m_data2Byte.emplace_back(temp);
			}
		}
		else
		{
			if (header.TuplType == "GRAYSCALE" && header.Depth == 1)
			{
				//GrayScale
				m_isImageGrayScale = true;
				m_bitsPerPixel = 8;
				m_format = ImageFormat::Gray_Scale;
			}
			else if (header.TuplType == "RGB" && header.Depth == 3)
			{
				//RGB
				m_isImageColored = true;
				m_bitsPerPixel = 24;
				m_format = ImageFormat::RGB;
			}
			else if (header.TuplType == "GRAYSCALE_ALPHA" && header.Depth == 2)
			{
				//GrayScaleAlpha
				m_isImageGrayScale = true;
				m_hasAlphaChannel = true;
				m_bitsPerPixel = 16;
				m_format = ImageFormat::Gray_Scale_Alpha;
			}
			else if (header.TuplType == "RGB_ALPHA" && header.Depth == 4)
			{
				//RGBA
				m_isImageColored = true;
				m_hasAlphaChannel = true;
				m_bitsPerPixel = 32;
				m_format = ImageFormat::RGBA;
			}

			m_data.reserve(m_width * m_height * header.Depth);
			m_data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		}

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::PAMImage::GetPixelData()
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PAMImage::GetPixelDataSize() const
{
	if (!m_data2Byte.empty())
		return static_cast<uint32_t>(m_data2Byte.size());

	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PAMImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PAMImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PAMImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PAMImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PAMImage::HasAlphaChannel() const
{
	return m_hasAlphaChannel;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PAMImage::IsImageCompressed() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PAMImage::IsImageGrayScale() const
{
	return m_isImageGrayScale;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PAMImage::IsImageColored() const
{
	return m_isImageColored;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PAMImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::PAMImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::PAMImage::GetFormat() const
{
	return m_format;
}