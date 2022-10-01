#include "TRAPPCH.h"
#include "PFMImage.h"

#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::PFMImage::PFMImage(std::filesystem::path filepath)
{
	ZoneScoped;

	m_filepath = std::move(filepath);
	m_isHDR = true;

	TP_DEBUG(Log::ImagePFMPrefix, "Loading image: \"", m_filepath.u8string(), "\"");

	if (!FileSystem::FileOrFolderExists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePFMPrefix, "Couldn't open file path: ", m_filepath.u8string(), "!");
		TP_WARN(Log::ImagePFMPrefix, "Using default image!");
		return;
	}

	//Read in file header
	Header header{};
	file >> header.MagicNumber >> header.Width >> header.Height >> header.ByteOrder;

	//Do error/specification checking
	if (header.MagicNumber != "PF" && header.MagicNumber != "Pf")
	{
		file.close();
		TP_ERROR(Log::ImagePFMPrefix, "Invalid magic number", header.MagicNumber, "!");
		TP_WARN(Log::ImagePFMPrefix, "Using default image!");
		return;
	}
	if (header.Width < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePFMPrefix, "Width is < 1 (", header.Width, ")!");
		TP_WARN(Log::ImagePFMPrefix, "Using default image!");
		return;
	}
	if (header.Height < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePFMPrefix, "Height is < 1 (", header.Height, ")!");
		TP_WARN(Log::ImagePFMPrefix, "Using default image!");
		return;
	}

	m_width = header.Width;
	m_height = header.Height;

	//Determine endianness
	const bool isFileLittleEndian = (header.ByteOrder < 0.0f); //If true little-endian is used else if false big-endian is used
	const bool needSwap = isFileLittleEndian != static_cast<bool>(Utils::GetEndian());

	file.ignore(256, '\n'); //Skip ahead to the pixel data

	if (header.MagicNumber == "PF")
	{
		//RGB
		m_colorFormat = ColorFormat::RGB;
		m_bitsPerPixel = 96;
		m_data.resize(static_cast<std::size_t>(m_width) * m_height * 3);
		if (!file.read(reinterpret_cast<char*>(m_data.data()),
					   static_cast<std::streamsize>(m_width) * m_height * 3 *
                       static_cast<std::streamsize>(sizeof(float))))
		{
			file.close();
			m_data.clear();
			TP_ERROR(Log::ImagePFMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePFMPrefix, "Using default image!");
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
		m_data.resize(static_cast<std::size_t>(m_width) * m_height);
		if (!file.read(reinterpret_cast<char*>(m_data.data()),
					   static_cast<std::streamsize>(m_width) * m_height *
					   static_cast<std::streamsize>(sizeof(float))))
		{
			file.close();
			m_data.clear();
			TP_ERROR(Log::ImagePFMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePFMPrefix, "Using default image!");
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
	ZoneScoped;

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::INTERNAL::PFMImage::GetPixelDataSize() const
{
	ZoneScoped;

	return m_data.size() * sizeof(float);
}