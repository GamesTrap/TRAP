#include "TRAPPCH.h"
#include "PNMImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"
#include "Utils/Expected.h"

namespace
{
	struct Header
	{
		std::string MagicNumber;
		u32 Width = 0;
		u32 Height = 0;
		u32 MaxValue = 255;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	enum class PNMErrorCode
	{
		InvalidMagicNumber,
		InvalidWidth,
		InvalidHeight,
		InvalidMaxValue,
		FailedToReadPixelData
	};

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::string PNMErrorCodeToString(const PNMErrorCode errorCode)
	{
		switch(errorCode)
		{
		case PNMErrorCode::InvalidMagicNumber:
			return "Invalid magic number detected, expected \"P2\", \"P3\", \"P5\" or \"P6\"!";

		case PNMErrorCode::InvalidWidth:
			return "Width is invalid!";

		case PNMErrorCode::InvalidHeight:
			return "Height is invalid!";

		case PNMErrorCode::InvalidMaxValue:
			return "MaxValue is invalid, expected value between [1, 65535)!";

		case PNMErrorCode::FailedToReadPixelData:
			return "Failed to read pixel data!";
		}

		TRAP_ASSERT(false);
		return "";
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<Header, PNMErrorCode> LoadHeader(std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		Header header{};
		file >> header.MagicNumber >> header.Width >> header.Height >> header.MaxValue;

		if (header.MagicNumber != "P2" && header.MagicNumber != "P5" &&
			header.MagicNumber != "P3" && header.MagicNumber != "P6")
		{
			return TRAP::MakeUnexpected(PNMErrorCode::InvalidMagicNumber);
		}
		if (header.Width < 1)
			return TRAP::MakeUnexpected(PNMErrorCode::InvalidWidth);
		if (header.Height < 1)
			return TRAP::MakeUnexpected(PNMErrorCode::InvalidHeight);
		if (header.MaxValue <= 0 || header.MaxValue > std::numeric_limits<u16>::max())
			return TRAP::MakeUnexpected(PNMErrorCode::InvalidMaxValue);

		return header;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u8>, PNMErrorCode> LoadPixelData8BPP(std::ifstream& file,
	                                                                              const u32 width, const u32 height,
																				  const u32 channels)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		std::vector<u8> pixelData(NumericCast<usize>(width) * height * channels);

		if (!file.read(reinterpret_cast<char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size())))
			return TRAP::MakeUnexpected(PNMErrorCode::FailedToReadPixelData);

		return pixelData;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u16>, PNMErrorCode> LoadPixelData16BPP(std::ifstream& file,
	                                                                                const u32 width, const u32 height,
																					const u32 channels)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		std::vector<u16> pixelData(NumericCast<usize>(width) * height * channels);
		if (!file.read(reinterpret_cast<char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size() * sizeof(u16))))
			return TRAP::MakeUnexpected(PNMErrorCode::FailedToReadPixelData);

		//File uses big-endian
		//Convert to machines endian
		if constexpr (TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
		{
			TRAP::Utils::Memory::SwapBytes(pixelData.begin(), pixelData.end());
		}

		return pixelData;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::PNMImage::PNMImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	TP_DEBUG(Log::ImagePNMPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePNMPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImagePNMPrefix, "Using default image!");
		return;
	}

	const auto header = LoadHeader(file);
	if(!header)
	{
		TP_ERROR(Log::ImagePNMPrefix, PNMErrorCodeToString(header.Error()));
		TP_WARN(Log::ImagePNMPrefix, "Using default image!");
		return;
	}

	m_width = header->Width;
	m_height = header->Height;

	file.ignore(256, '\n'); //Skip ahead to the pixel data.

	if(header->MaxValue > 255)
	{
		if (header->MagicNumber == "P2" || header->MagicNumber == "P5")
		{
			//GrayScale
			m_colorFormat = ColorFormat::GrayScale;
			m_bitsPerPixel = 16;
		}
		else if (header->MagicNumber == "P3" || header->MagicNumber == "P6")
		{
			m_colorFormat = ColorFormat::RGB;
			m_bitsPerPixel = 48;
		}

		const auto pixelData = LoadPixelData16BPP(file, m_width, m_height, m_bitsPerPixel / 16u);
		if(!pixelData)
		{
			TP_ERROR(Log::ImagePNMPrefix, PNMErrorCodeToString(pixelData.Error()));
			TP_WARN(Log::ImagePNMPrefix, "Using default image!");
			return;
		}

		m_data2Byte = *pixelData;
	}
	else
	{
		if (header->MagicNumber == "P2" || header->MagicNumber == "P5")
		{
			m_colorFormat = ColorFormat::GrayScale;
			m_bitsPerPixel = 8;
		}
		else if (header->MagicNumber == "P3" || header->MagicNumber == "P6")
		{
			m_colorFormat = ColorFormat::RGB;
			m_bitsPerPixel = 24;
		}

		const auto pixelData = LoadPixelData8BPP(file, m_width, m_height, m_bitsPerPixel / 8u);
		if(!pixelData)
		{
			TP_ERROR(Log::ImagePNMPrefix, PNMErrorCodeToString(pixelData.Error()));
			TP_WARN(Log::ImagePNMPrefix, "Using default image!");
			return;
		}

		m_data = *pixelData;
	}
}
