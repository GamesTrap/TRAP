#include "TRAPPCH.h"
#include "PPMImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::PPMImage::PPMImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	m_colorFormat = ColorFormat::RGB;

	TP_DEBUG(Log::ImagePPMPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePPMPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImagePPMPrefix, "Using default image!");
		return;
	}

	Header header{};
	file >> header.MagicNumber >> header.Width >> header.Height >> header.MaxValue;

	if (header.MagicNumber != "P3" && header.MagicNumber != "P6")
	{
		file.close();
		TP_ERROR(Log::ImagePPMPrefix, "Unsupported format or invalid magic number (", header.MagicNumber, ")!");
		TP_WARN(Log::ImagePPMPrefix, "Using default image!");
		return;
	}
	if (header.Width < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePPMPrefix, "Width is < 1 (", header.Width, ")!");
		TP_WARN(Log::ImagePPMPrefix, "Using default image!");
		return;
	}
	if (header.Height < 1)
	{
		file.close();
		TP_ERROR(Log::ImagePPMPrefix, "Height is < 1 (", header.Height, ")!");
		TP_WARN(Log::ImagePPMPrefix, "Using default image!");
		return;
	}
	if (header.MaxValue < 1 || header.MaxValue > 65535)
	{
		file.close();
		TP_ERROR(Log::ImagePPMPrefix, "MaxValue ", header.MaxValue, " is unsupported or invalid!");
		TP_WARN(Log::ImagePPMPrefix, "Using default image!");
		return;
	}

	m_width = header.Width;
	m_height = header.Height;

	file.ignore(256, '\n'); //Skip ahead to the pixel data.

	if(header.MaxValue > 255)
	{
		m_bitsPerPixel = 48;
		m_data2Byte.resize(NumericCast<usize>(m_width) * m_height * 3);
		if(!file.read(reinterpret_cast<char*>(m_data2Byte.data()),
		              NumericCast<std::streamsize>(m_width) * m_height * 3 *
					  NumericCast<std::streamsize>(sizeof(u16))))
		{
			file.close();
			TP_ERROR(Log::ImagePPMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePPMPrefix, "Using default image!");
			return;
		}

		file.close();

		//File uses big-endian
		//Convert to machines endian
		if constexpr (Utils::GetEndian() != Utils::Endian::Big)
		{
			for (u16& element : m_data2Byte)
				Utils::Memory::SwapBytes(element);
		}
	}
	else
	{
		m_bitsPerPixel = 24;
		m_data.resize(NumericCast<usize>(m_width) * m_height * 3);
		if (!file.read(reinterpret_cast<char*>(m_data.data()),
					   NumericCast<std::streamsize>(m_width) * m_height * 3))
		{
			file.close();
			TP_ERROR(Log::ImagePPMPrefix, "Couldn't load pixel data!");
			TP_WARN(Log::ImagePPMPrefix, "Using default image!");
			return;
		}

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::PPMImage::Save(const Image* const img, const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	//NOTE Only supports 24/32BPP RGB(A) Input

	std::ofstream file(filepath, std::ios::out | std::ios::binary);

	//PPM Header
	file << fmt::format("P6\n{}\n{}\n255\n", img->GetWidth(), img->GetHeight());

	//PPM Data
	std::vector<u8> pixelData;
	if(img->GetColorFormat() == ColorFormat::RGBA)
	{
		pixelData = ConvertRGBAToRGB<u8>(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const u8*>(img->GetPixelData()));
		file.write(reinterpret_cast<const char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size()));
	}
	else
		file.write(static_cast<const char*>(img->GetPixelData()), NumericCast<std::streamsize>(img->GetPixelDataSize()));

	file.close();
}