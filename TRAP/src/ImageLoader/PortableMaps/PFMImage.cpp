#include "TRAPPCH.h"
#include "PFMImage.h"

#include "Utils/String/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"
#include "Application.h"
#include "Utils/ByteSwap.h"

TRAP::INTERNAL::PFMImage::PFMImage(std::string filepath)
{
	TP_PROFILE_FUNCTION();

	m_filepath = std::move(filepath);
	m_isHDR = true;

	TP_DEBUG("[Image][PFM] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::SilentResolveReadPhysicalPath(m_filepath, physicalPath))
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
			m_colorFormat = ColorFormat::RGB;
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
			m_colorFormat = ColorFormat::GrayScale;
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

const void* TRAP::INTERNAL::PFMImage::GetPixelData() const
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PFMImage::GetPixelDataSize() const
{
	return static_cast<uint32_t>(m_data.size());
}