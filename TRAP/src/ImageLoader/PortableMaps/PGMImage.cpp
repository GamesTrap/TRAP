#include "TRAPPCH.h"
#include "PGMImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"
#include "Utils/Expected.h"

namespace
{
	struct Header
	{
		std::string MagicNumber;
		u32 Width = 0u;
		u32 Height = 0u;
		u32 MaxValue = 255u;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	enum class PGMErrorCode
	{
		InvalidMagicNumber,
		InvalidWidth,
		InvalidHeight,
		InvalidMaxValue,
		FailedToReadPixelData
	};

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::string PGMErrorCodeToString(const PGMErrorCode errorCode)
	{
		switch(errorCode)
		{
		case PGMErrorCode::InvalidMagicNumber:
			return "Invalid magic number detected, expected \"P2\" or \"P5\"!";

		case PGMErrorCode::InvalidWidth:
			return "Width is invalid!";

		case PGMErrorCode::InvalidHeight:
			return "Height is invalid!";

		case PGMErrorCode::InvalidMaxValue:
			return "MaxValue is invalid, expected value between [1, 65535)!";

		case PGMErrorCode::FailedToReadPixelData:
			return "Failed to read pixel data!";
		}

		TRAP_ASSERT(false);
		return "";
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<Header, PGMErrorCode> LoadHeader(std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		const auto SkipComments = [&file]()
		{
			std::string tmp{'#'};
			while(!tmp.empty() && tmp.front() == '#')
				file >> tmp;
			return tmp;
		};

		Header header{};
		header.MagicNumber = SkipComments();
		header.Width = std::stoi(SkipComments());
		header.Height = std::stoi(SkipComments());
		header.MaxValue = std::stoi(SkipComments());

		if (!(header.MagicNumber == "P2" || header.MagicNumber == "P5"))
			return TRAP::MakeUnexpected(PGMErrorCode::InvalidMagicNumber);
		if (header.Width < 1u)
			return TRAP::MakeUnexpected(PGMErrorCode::InvalidWidth);
		if (header.Height < 1u)
			return TRAP::MakeUnexpected(PGMErrorCode::InvalidHeight);
		if(header.MaxValue <= 0u || header.MaxValue > std::numeric_limits<u16>::max())
			return TRAP::MakeUnexpected(PGMErrorCode::InvalidMaxValue);

		return header;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u8>, PGMErrorCode> LoadPixelData8BPP(std::ifstream& file,
	                                                                             const u32 width, const u32 height)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		std::vector<u8> pixelData(NumericCast<usize>(width) * height);

		if(!file.read(reinterpret_cast<char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size())))
			return TRAP::MakeUnexpected(PGMErrorCode::FailedToReadPixelData);

		return pixelData;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u16>, PGMErrorCode> LoadPixelData16BPP(std::ifstream& file,
	                                                                               const u32 width, const u32 height)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		std::vector<u16> pixelData(NumericCast<usize>(width) * height);

		if(!file.read(reinterpret_cast<char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size() * sizeof(u16))))
			return TRAP::MakeUnexpected(PGMErrorCode::FailedToReadPixelData);

		file.close();

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

TRAP::INTERNAL::PGMImage::PGMImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	m_colorFormat = ColorFormat::GrayScale;

	TP_DEBUG(Log::ImagePGMPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePGMPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImagePGMPrefix, "Using default image!");
		return;
	}

	const auto header = LoadHeader(file);
	if(!header)
	{
		TP_ERROR(Log::ImagePGMPrefix, PGMErrorCodeToString(header.Error()));
		TP_WARN(Log::ImagePGMPrefix, "Using default image!");
		return;
	}

	m_width = header->Width;
	m_height = header->Height;

	file.ignore(256, '\n'); //Skip ahead to the pixel data.

	if(header->MaxValue > 255u)
	{
		m_bitsPerPixel = 16u;

		const auto pixelData = LoadPixelData16BPP(file, m_width, m_height);
		if(!pixelData)
		{
			TP_ERROR(Log::ImagePGMPrefix, PGMErrorCodeToString(pixelData.Error()));
			TP_WARN(Log::ImagePGMPrefix, "Using default image!");
			return;
		}

		m_data2Byte = *pixelData;
	}
	else
	{
		m_bitsPerPixel = 8u;

		const auto pixelData = LoadPixelData8BPP(file, m_width, m_height);
		if(!pixelData)
		{
			TP_ERROR(Log::ImagePGMPrefix, PGMErrorCodeToString(pixelData.Error()));
			TP_WARN(Log::ImagePGMPrefix, "Using default image!");
			return;
		}

		m_data = *pixelData;
	}
}
