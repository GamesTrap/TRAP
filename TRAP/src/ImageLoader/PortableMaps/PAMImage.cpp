#include "TRAPPCH.h"
#include "PAMImage.h"

#include "Utils/String/String.h"
#include "VFS/VFS.h"
#include "Application.h"
#include "Utils/ByteSwap.h"

TRAP::INTERNAL::PAMImage::PAMImage(std::string filepath)
{
	TP_PROFILE_FUNCTION();

	m_filepath = std::move(filepath);

	TP_DEBUG(Log::ImagePAMPrefix, "Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::ResolveReadPhysicalPath(m_filepath, physicalPath, true))
	{
		TP_ERROR(Log::ImagePAMPrefix, "Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN(Log::ImagePAMPrefix, "Using Default Image!");
		return;
	}

	if (VFS::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR(Log::ImagePAMPrefix, "Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN(Log::ImagePAMPrefix, "Using Default Image!");
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
		
		if (header.MagicNumber != "P7")
		{
			file.close();
			TP_ERROR(Log::ImagePAMPrefix, "Invalid Magic Number!");
			TP_WARN(Log::ImagePAMPrefix, "Using Default Image!");
			return;
		}
		if (header.Width < 1)
		{
			file.close();
			TP_ERROR(Log::ImagePAMPrefix, "Width is < 1!");
			TP_WARN(Log::ImagePAMPrefix, "Using Default Image!");
			return;
		}
		if (header.Height < 1)
		{
			file.close();
			TP_ERROR(Log::ImagePAMPrefix, "Height is < 1!");
			TP_WARN(Log::ImagePAMPrefix, "Using Default Image!");
			return;
		}
		if (header.MaxValue < 1 || header.MaxValue > 65535)
		{
			file.close();
			TP_ERROR(Log::ImagePAMPrefix, "MaxValue is unsupported/invalid!");
			TP_WARN(Log::ImagePAMPrefix, "Using Default Image!");
			return;
		}
		if (!(header.TuplType == "GRAYSCALE" || header.TuplType == "RGB" || header.TuplType == "GRAYSCALE_ALPHA" || header.TuplType == "RGB_ALPHA"))
		{
			file.close();
			TP_ERROR(Log::ImagePAMPrefix, "TuplType is unsupported or invalid!");
			TP_WARN(Log::ImagePAMPrefix, "Using Default Image!");
			return;
		}
		
		m_width = header.Width;
		m_height = header.Height;
		
		file.ignore(256, '\n'); //Skip ahead to the pixel data.

		if(header.MaxValue > 255)
		{
			if (header.TuplType == "GRAYSCALE" && header.Depth == 1)
			{
				//GrayScale
				m_bitsPerPixel = 16;
				m_colorFormat = ColorFormat::GrayScale;
			}
			else if (header.TuplType == "RGB" && header.Depth == 3)
			{
				//RGB
				m_bitsPerPixel = 48;
				m_colorFormat = ColorFormat::RGB;
			}
			else if (header.TuplType == "GRAYSCALE_ALPHA" && header.Depth == 2)
			{
				//GrayScaleAlpha
				m_bitsPerPixel = 32;
				m_colorFormat = ColorFormat::GrayScaleAlpha;
			}
			else if (header.TuplType == "RGB_ALPHA" && header.Depth == 4)
			{
				//RGBA
				m_bitsPerPixel = 64;
				m_colorFormat = ColorFormat::RGBA;
			}

			m_data2Byte.resize(m_width * m_height * header.Depth);
			if (!file.read(reinterpret_cast<char*>(m_data2Byte.data()), m_width * m_height * header.Depth * sizeof(uint16_t)))
			{
				file.close();
				m_data2Byte.clear();
				TP_ERROR(Log::ImagePAMPrefix, "Couldn't load pixel data!");
				TP_WARN(Log::ImagePAMPrefix, "Using Default Image!");
				return;
			}

			//File uses big-endian
			//Convert to machines endian
			bool needSwap = static_cast<bool>(Application::GetEndian() != Application::Endian::Big);
			if (needSwap)
				for (uint16_t& element : m_data2Byte)
					Utils::Memory::SwapBytes(element);
		}
		else
		{
			if (header.TuplType == "GRAYSCALE" && header.Depth == 1)
			{
				//GrayScale
				m_bitsPerPixel = 8;
				m_colorFormat = ColorFormat::GrayScale;
			}
			else if (header.TuplType == "RGB" && header.Depth == 3)
			{
				//RGB
				m_bitsPerPixel = 24;
				m_colorFormat = ColorFormat::RGB;
			}
			else if (header.TuplType == "GRAYSCALE_ALPHA" && header.Depth == 2)
			{
				//GrayScaleAlpha
				m_bitsPerPixel = 16;
				m_colorFormat = ColorFormat::GrayScaleAlpha;
			}
			else if (header.TuplType == "RGB_ALPHA" && header.Depth == 4)
			{
				//RGBA
				m_bitsPerPixel = 32;
				m_colorFormat = ColorFormat::RGBA;
			}

			m_data.resize(m_width * m_height * header.Depth);
			if(!file.read(reinterpret_cast<char*>(m_data.data()), m_width * m_height * header.Depth))
			{
				file.close();
				m_data.clear();
				TP_ERROR(Log::ImagePAMPrefix, "Couldn't load pixel data!");
				TP_WARN(Log::ImagePAMPrefix, "Using Default Image!");
				return;
			}
		}

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::INTERNAL::PAMImage::GetPixelData() const
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