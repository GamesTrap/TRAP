#include "TRAPPCH.h"
#include "PFMImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"
#include "Application.h"
#include "Utils/ByteSwap.h"

TRAP::INTERNAL::PFMImage::PFMImage(std::string filepath)
	: m_filepath(std::move(filepath)), m_bitsPerPixel(0), m_isImageGrayScale(false), m_isImageColored(false), m_width(0), m_height(0), m_format(ImageFormat::NONE)
{
	TP_DEBUG("[Image][PFM] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TP_ERROR("[Image][PFM] Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN("[Image][PFM] Using Default Image!");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR("[Image][PFM] Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN("[Image][PFM] Using Default Image!");
			return;
		}

		//Read in file header
		struct Header
		{
			std::string MagicNumber = "";
			uint32_t Width = 0;
			uint32_t Height = 0;
			float ByteOrder = 0.0f;
		} header;
		file >> header.MagicNumber >> header.Width >> header.Height >> header.ByteOrder;
		
		//Do error/specification checking
		if (header.MagicNumber != "PF" && header.MagicNumber != "Pf")
		{
			file.close();
			TP_ERROR("[Image][PFM] Invalid Magic Number!");
			TP_WARN("[Image][PFM] Using Default Image!");
			return;
		}
		if (header.Width < 1)
		{
			file.close();
			TP_ERROR("[Image][PFM] Width is < 1!");
			TP_WARN("[Image][PFM] Using Default Image!");
			return;
		}
		if (header.Height < 1)
		{
			file.close();
			TP_ERROR("[Image][PFM] Height is < 1!");
			TP_WARN("[Image][PFM] Using Default Image!");
			return;
		}

		m_width = header.Width;
		m_height = header.Height;
		
		//Determine endianess
		bool isFileLittleEndian = (header.ByteOrder < 0.0f); //If true little-endian is used else if false big-endian is used
		bool needSwap = isFileLittleEndian != static_cast<bool>(Application::GetEndian());
		
		file.ignore(256, '\n'); //Skip ahead to the pixel data	

		if (header.MagicNumber == "PF")
		{
			//RGB
			m_format = ImageFormat::RGB;
			m_isImageColored = true;
			m_bitsPerPixel = 96;
			m_data.resize(m_width * m_height * 3);
			if (!file.read(reinterpret_cast<char*>(m_data.data()), m_width * m_height * 3 * sizeof(float)))
			{
				file.close();
				m_data.clear();
				TP_ERROR("[Image][PFM] Couldn't load pixel data!");
				TP_WARN("[Image][PFM] Using Default Image!");
				return;
			}

			if (needSwap)
				for (float& element : m_data)
					Utils::Memory::SwapBytes(element);
		}
		else if(header.MagicNumber == "Pf")
		{
			//GrayScale
			m_format = ImageFormat::Gray_Scale;
			m_isImageGrayScale = true;
			m_bitsPerPixel = 32;
			m_data.resize(m_width* m_height);
			if (!file.read(reinterpret_cast<char*>(m_data.data()), m_width * m_height * sizeof(float)))
			{
				file.close();
				m_data.clear();
				TP_ERROR("[Image][PFM] Couldn't load pixel data!");
				TP_WARN("[Image][PFM] Using Default Image!");
				return;
			}

			if (needSwap)
				for (float& element : m_data)
					Utils::Memory::SwapBytes(element);
		}

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::PFMImage::GetPixelData()
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PFMImage::GetPixelDataSize() const
{
	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PFMImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PFMImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PFMImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PFMImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PFMImage::HasAlphaChannel() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PFMImage::IsImageGrayScale() const
{
	return m_isImageGrayScale;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PFMImage::IsImageColored() const
{
	return m_isImageColored;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PFMImage::IsHDR() const
{
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::INTERNAL::PFMImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::PFMImage::GetFormat() const
{
	return m_format;
}