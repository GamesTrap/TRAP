#include "TRAPPCH.h"
#include "PNMImage.h"

#include "Utils/String/String.h"
#include "VFS/VFS.h"
#include "Application.h"
#include "Utils/ByteSwap.h"

TRAP::INTERNAL::PNMImage::PNMImage(std::string filepath)
{
	TP_PROFILE_FUNCTION();

	m_filepath = std::move(filepath);

	TP_DEBUG(Log::ImagePNMPrefix, "Loading Image: \"", Utils::String::SplitStringView(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::ResolveReadPhysicalPath(m_filepath, physicalPath, true))
	{
		TP_ERROR(Log::ImagePNMPrefix, "Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
		return;
	}

	if (VFS::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR(Log::ImagePNMPrefix, "Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
			return;
		}

		struct Header
		{
			std::string MagicNumber = "";
			std::uint32_t Width = 0;
			std::uint32_t Height = 0;
			std::uint32_t MaxValue = 255;
		} header;
		file >> header.MagicNumber >> header.Width >> header.Height >> header.MaxValue;

		if (!(header.MagicNumber == "P2" || header.MagicNumber == "P5" || header.MagicNumber == "P3" || header.MagicNumber == "P6"))
		{
			file.close();
			TP_ERROR(Log::ImagePNMPrefix, "Unsupported Format or invalid Magic Number!");
			TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
			return;
		}
		if (header.Width < 1)
		{
			file.close();
			TP_ERROR(Log::ImagePNMPrefix, "Width is < 1!");
			TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
			return;
		}
		if (header.Height < 1)
		{
			file.close();
			TP_ERROR(Log::ImagePNMPrefix, "Height is < 1!");
			TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
			return;
		}
		if (header.MaxValue < 1 || header.MaxValue > 65535)
		{
			file.close();
			TP_ERROR(Log::ImagePNMPrefix, "Max Value is unsupported/invalid!");
			TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
			return;
		}

		m_width = header.Width;
		m_height = header.Height;

		file.ignore(256, '\n'); //Skip ahead to the pixel data.
		
		if(header.MaxValue > 255)
		{
			if (header.MagicNumber == "P2" || header.MagicNumber == "P5")
			{
				//GrayScale
				m_colorFormat = ColorFormat::GrayScale;
				m_bitsPerPixel = 16;
				m_data2Byte.resize(m_width * m_height);
				if(!file.read(reinterpret_cast<char*>(m_data2Byte.data()), m_width * m_height * sizeof(uint16_t)))
				{
					file.close();
					TP_ERROR(Log::ImagePNMPrefix, "Couldn't load pixel data!");
					TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
					return;
				}
			}
			else if (header.MagicNumber == "P3" || header.MagicNumber == "P6")
			{
				//RGB
				m_colorFormat = ColorFormat::RGB;
				m_bitsPerPixel = 48;
				m_data2Byte.resize(m_width * m_height * 3);
				if (!file.read(reinterpret_cast<char*>(m_data2Byte.data()), m_width * m_height * 3 * sizeof(uint16_t)))
				{
					file.close();
					TP_ERROR(Log::ImagePNMPrefix, "Couldn't load pixel data!");
					TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
					return;
				}
			}

			file.close();

			//File uses big-endian
			//Convert to machines endian
			bool needSwap = static_cast<bool>(Application::GetEndian() != Application::Endian::Big);
			if (needSwap)
				for (uint16_t& element : m_data2Byte)
					Utils::Memory::SwapBytes(element);
		}
		else 
		{
			if (header.MagicNumber == "P2" || header.MagicNumber == "P5")
			{
				//GrayScale
				m_colorFormat = ColorFormat::GrayScale;
				m_bitsPerPixel = 8;
				m_data.resize(m_width * m_height);
				if(!file.read(reinterpret_cast<char*>(m_data.data()), m_width * m_height))
				{
					file.close();
					TP_ERROR(Log::ImagePNMPrefix, "Couldn't load pixel data!");
					TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
					return;
				}
			}
			else if (header.MagicNumber == "P3" || header.MagicNumber == "P6")
			{
				//RGB
				m_colorFormat = ColorFormat::RGB;
				m_bitsPerPixel = 24;
				m_data.resize(m_width * m_height * 3);
				if (!file.read(reinterpret_cast<char*>(m_data.data()), m_width * m_height * 3))
				{
					file.close();
					TP_ERROR(Log::ImagePNMPrefix, "Couldn't load pixel data!");
					TP_WARN(Log::ImagePNMPrefix, "Using Default Image!");
					return;
				}
			}

			file.close();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::INTERNAL::PNMImage::GetPixelData() const
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::INTERNAL::PNMImage::GetPixelDataSize() const
{
	if (!m_data2Byte.empty())
		return m_data2Byte.size() * sizeof(uint16_t);

	return m_data.size();
}