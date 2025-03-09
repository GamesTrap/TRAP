#include "TRAPPCH.h"
#include "PAMImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"

namespace
{
	struct Header
	{
		std::string MagicNumber;
		u32 Width = 0u;
		u32 Height = 0u;
		u32 Depth = 0u;
		u32 MaxValue = 255u;
		std::string TuplType;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	enum class PAMErrorCode : u8
	{
		InvalidMagicNumber,
		InvalidWidth,
		InvalidHeight,
		InvalidDepth,
		InvalidMaxValue,
		InvalidTuplType,
		InvalidTuplTypeAndDepthCombination,
		InvalidHeaderEnd,
		UnsupportedTuplTypeBlackAndWhite,
		FailedToReadPixelData
	};

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::string PAMErrorCodeToString(const PAMErrorCode errorCode)
	{
		switch(errorCode)
		{
		case PAMErrorCode::InvalidMagicNumber:
			return "Invalid magic number detected, expected \"P7\"!";

		case PAMErrorCode::InvalidWidth:
			return "Width is invalid!";

		case PAMErrorCode::InvalidHeight:
			return "Height is invalid!";

		case PAMErrorCode::InvalidDepth:
			return "Depth is invalid!";

		case PAMErrorCode::InvalidMaxValue:
			return "Max value is invalid!";

		case PAMErrorCode::InvalidTuplType:
			return "TuplType is invalid, expected one of \"GRAYSCALE\", \"GRAYSCALE_ALPHA\", \"RGB\", \"RGB_ALPHA\"!";

		case PAMErrorCode::InvalidTuplTypeAndDepthCombination:
			return "TuplType and depth combination is invalid!";

		case PAMErrorCode::InvalidHeaderEnd:
			return "Header end is invalid, expected \"ENDHDR\"!";

		case PAMErrorCode::UnsupportedTuplTypeBlackAndWhite:
			return "TuplType \"BLACKANDWHITE\" & \"BLACKANDWHITE_ALPHA\" are unsupported!";

		case PAMErrorCode::FailedToReadPixelData:
			return "Failed to read pixel data!";
		}

		TRAP_ASSERT(false);
		return "<UNKNOWN PAMErrorCode>";
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<Header, PAMErrorCode> LoadHeader(std::ifstream& file)
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
		[[maybe_unused]] const std::string widthStr = SkipComments();
		const std::string widthValueStr = SkipComments();
		if(std::from_chars(widthValueStr.data(), widthValueStr.data() + widthValueStr.size(), header.Width).ec != std::errc{})
			header.Width = 0u;
		[[maybe_unused]] const std::string heightStr = SkipComments();
		const std::string heightValueStr = SkipComments();
		if(std::from_chars(heightValueStr.data(), heightValueStr.data() + heightValueStr.size(), header.Height).ec != std::errc{})
			header.Height = 0u;
		[[maybe_unused]] const std::string depthStr = SkipComments();
		const std::string depthValueStr = SkipComments();
		if(std::from_chars(depthValueStr.data(), depthValueStr.data() + depthValueStr.size(), header.Depth).ec != std::errc{})
			header.Depth = 0u;
		[[maybe_unused]] const std::string maxValStr = SkipComments();
		const std::string maxValValueStr = SkipComments();
		if(std::from_chars(maxValValueStr.data(), maxValValueStr.data() + maxValValueStr.size(), header.MaxValue).ec != std::errc{})
			header.MaxValue = std::numeric_limits<u32>::max();
		[[maybe_unused]] const std::string tuplTypeStr = SkipComments();
		header.TuplType = SkipComments();
		const std::string endHdrStr = SkipComments();

		if (header.MagicNumber != "P7")
			return TRAP::MakeUnexpected(PAMErrorCode::InvalidMagicNumber);
		if (header.Width < 1u)
			return TRAP::MakeUnexpected(PAMErrorCode::InvalidWidth);
		if (header.Height < 1u)
			return TRAP::MakeUnexpected(PAMErrorCode::InvalidHeight);
		if (header.MaxValue <= 0u || header.MaxValue > std::numeric_limits<u16>::max())
			return TRAP::MakeUnexpected(PAMErrorCode::InvalidMaxValue);
		if (header.Depth <= 0u || header.Depth > 4u)
			return TRAP::MakeUnexpected(PAMErrorCode::InvalidDepth);
		if(header.TuplType == "BLACKANDWHITE" || header.TuplType == "BLACKANDWHITE_ALPHA")
			return TRAP::MakeUnexpected(PAMErrorCode::UnsupportedTuplTypeBlackAndWhite);
		if (header.TuplType != "GRAYSCALE" && header.TuplType != "RGB" &&
			header.TuplType != "GRAYSCALE_ALPHA" && header.TuplType != "RGB_ALPHA")
		{
			return TRAP::MakeUnexpected(PAMErrorCode::InvalidTuplType);
		}
		if((header.TuplType == "GRAYSCALE" && header.Depth != 1u) ||
		   (header.TuplType == "GRAYSCALE_ALPHA" && header.Depth != 2u) ||
		   (header.TuplType == "RGB" && header.Depth != 3u) ||
		   (header.TuplType == "RGB_ALPHA" && header.Depth != 4u))
		{
			return TRAP::MakeUnexpected(PAMErrorCode::InvalidTuplTypeAndDepthCombination);
		}
		if(endHdrStr != "ENDHDR")
			return TRAP::MakeUnexpected(PAMErrorCode::InvalidHeaderEnd);

		return header;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename T>
	requires std::same_as<T, u8> || std::same_as<T, u16>
	[[nodiscard]] TRAP::Expected<std::vector<T>, PAMErrorCode> LoadPixelData(std::ifstream& file, const u32 width, const u32 height, const u32 channels)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		std::vector<T> pixelData(NumericCast<usize>(width) * height * channels);

		if (!file.read(reinterpret_cast<char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size() * sizeof(T))))
			return TRAP::MakeUnexpected(PAMErrorCode::FailedToReadPixelData);

		file.close();

		//File uses big-endian
		//Convert to machines endian
		if constexpr (std::same_as<T, u16> && TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
		{
			TRAP::Utils::Memory::SwapBytes(pixelData.begin(), pixelData.end());
		}

		return pixelData;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::PAMImage::PAMImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	TP_DEBUG(Log::ImagePAMPrefix, "Loading image: ", m_filepath, "");

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePAMPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImagePAMPrefix, "Using default image!");
		return;
	}

	const auto header = LoadHeader(file);
	if(!header)
	{
		TP_ERROR(Log::ImagePAMPrefix, PAMErrorCodeToString(header.Error()));
		TP_WARN(Log::ImagePAMPrefix, "Using default image!");
		return;
	}

	m_width = header->Width;
	m_height = header->Height;

	if (header->Depth == 1u)
		m_colorFormat = ColorFormat::GrayScale;
	else if (header->Depth == 3u)
		m_colorFormat = ColorFormat::RGB;
	else if (header->Depth == 2u)
		m_colorFormat = ColorFormat::GrayScaleAlpha;
	else if (header->Depth == 4u)
		m_colorFormat = ColorFormat::RGBA;

	file.ignore(256, '\n'); //Skip ahead to the pixel data.

	if(header->MaxValue > 255)
	{
		m_bitsPerPixel = header->Depth * sizeof(u16) * 8u;

		const auto pixelData = LoadPixelData<u16>(file, m_width, m_height, header->Depth);
		if(!pixelData)
		{
			TP_ERROR(Log::ImagePAMPrefix, PAMErrorCodeToString(pixelData.Error()));
			TP_WARN(Log::ImagePAMPrefix, "Using default image!");
			return;
		}

		m_data2Byte = *pixelData;
	}
	else
	{
		m_bitsPerPixel = header->Depth * 8u;

		const auto pixelData = LoadPixelData<u8>(file, m_width, m_height, header->Depth);
		if(!pixelData)
		{
			TP_ERROR(Log::ImagePAMPrefix, PAMErrorCodeToString(pixelData.Error()));
			TP_WARN(Log::ImagePAMPrefix, "Using default image!");
			return;
		}

		m_data = *pixelData;
	}
}
