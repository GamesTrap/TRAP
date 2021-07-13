#include "TRAPPCH.h"
#include "PFMImage.h"

#include "Utils/String/String.h"
#include "VFS/VFS.h"
#include "Utils/ByteSwap.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::PFMImage::PFMImage(std::string filepath)
{
	TP_PROFILE_FUNCTION();

	m_filepath = std::move(filepath);
	m_isHDR = true;

	TP_DEBUG(Log::ImagePFMPrefix, "Loading Image: \"", Utils::String::SplitStringView(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::ResolveReadPhysicalPath(m_filepath, physicalPath, true))
	{
		TP_ERROR(Log::ImagePFMPrefix, "Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN(Log::ImagePFMPrefix, "Using Default Image!");
		return;
	}

	if (!VFS::FileOrFolderExists(physicalPath))
		return;

	std::ifstream file(physicalPath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePFMPrefix, "Couldn't open FilePath: ", m_filepath, "!");
		TP_WARN(Log::ImagePFMPrefix, "Using Default Image!");
		return;
	}

	//Read in file header
	Header header{};
	file >> header.MagicNumber >> header.Width >> header.Height >> header.ByteOrder;

	//Do error/specification checking
	if (header.MagicNumber != "PF" && header.MagicNumber != "Pf")
	{
		file.close();
		TP_ERROR(Log::ImagePFMPrefix, "Invalid Magic Number!");
		TP_WARN(Log::ImagePFMPrefix, "Using Default Image!");
		return;
	}
	if (header.Width < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePFMPrefix, "Width is < 1!");
		TP_WARN(Log::ImagePFMPrefix, "Using Default Image!");
		return;
	}
	if (header.Height < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePFMPrefix, "Height is < 1!");
		TP_WARN(Log::ImagePFMPrefix, "Using Default Image!");
		return;
	}

	m_width = header.Width;
	m_height = header.Height;

	//Determine endianness
	bool isFileLittleEndian = (header.ByteOrder < 0.0f); //If true little-endian is used else if false big-endian is used
	bool needSwap = isFileLittleEndian != static_cast<bool>(Utils::GetEndian());

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
			TP_ERROR(Log::ImagePFMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePFMPrefix, "Using Default Image!");
			return;
		}

		file.close();

		if (needSwap)
		{
			for (float& element : m_data)
				Utils::Memory::SwapBytes(element);
		}
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
			TP_ERROR(Log::ImagePFMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePFMPrefix, "Using Default Image!");
			return;
		}

		file.close();

		if (needSwap)
		{
			for (float& element : m_data)
				Utils::Memory::SwapBytes(element);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::INTERNAL::PFMImage::GetPixelData() const
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::INTERNAL::PFMImage::GetPixelDataSize() const
{
	return m_data.size() * sizeof(float);
}