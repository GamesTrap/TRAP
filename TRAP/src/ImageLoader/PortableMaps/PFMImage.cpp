#include "TRAPPCH.h"
#include "PFMImage.h"

#include "FileSystem/FileSystem.h"

TRAP::INTERNAL::PFMImage::PFMImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	m_isHDR = true;

	TP_DEBUG(Log::ImagePFMPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePFMPrefix, "Couldn't open file path: ", m_filepath, "!");
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

	file.ignore(256, '\n'); //Skip ahead to the pixel data

	if (header.MagicNumber == "PF")
	{
		//RGB
		m_colorFormat = ColorFormat::RGB;
		m_bitsPerPixel = 96;
		m_data.resize(NumericCast<usize>(m_width) * m_height * 3);
		if (!file.read(reinterpret_cast<char*>(m_data.data()),
					   NumericCast<std::streamsize>(m_width) * m_height * 3 *
                       NumericCast<std::streamsize>(sizeof(f32))))
		{
			file.close();
			m_data.clear();
			TP_ERROR(Log::ImagePFMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePFMPrefix, "Using default image!");
			return;
		}

		file.close();
	}
	else if(header.MagicNumber == "Pf")
	{
		//GrayScale
		m_colorFormat = ColorFormat::GrayScale;
		m_bitsPerPixel = 32;
		m_data.resize(NumericCast<usize>(m_width) * m_height);
		if (!file.read(reinterpret_cast<char*>(m_data.data()),
					   NumericCast<std::streamsize>(m_width) * m_height *
					   NumericCast<std::streamsize>(sizeof(f32))))
		{
			file.close();
			m_data.clear();
			TP_ERROR(Log::ImagePFMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePFMPrefix, "Using default image!");
			return;
		}

		file.close();
	}
}
