#include "TRAPPCH.h"
#include "PPMImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/ImageUtils.h"
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

	enum class PPMErrorCode : u8
	{
		InvalidMagicNumber,
		InvalidWidth,
		InvalidHeight,
		InvalidMaxValue,
		FailedToReadPixelData
	};

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::string PPMErrorCodeToString(const PPMErrorCode errorCode)
	{
		switch(errorCode)
		{
		case PPMErrorCode::InvalidMagicNumber:
			return "Invalid magic number detected, expected \"P3\" or \"P6\"!";

		case PPMErrorCode::InvalidWidth:
			return "Width is invalid!";

		case PPMErrorCode::InvalidHeight:
			return "Height is invalid!";

		case PPMErrorCode::InvalidMaxValue:
			return "MaxValue is invalid, expected value between [1, 65535)!";

		case PPMErrorCode::FailedToReadPixelData:
			return "Failed to read pixel data!";
		}

		TRAP_ASSERT(false);
		return "";
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<Header, PPMErrorCode> LoadHeader(std::ifstream& file)
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

		if (!(header.MagicNumber == "P3" || header.MagicNumber == "P6"))
			return TRAP::MakeUnexpected(PPMErrorCode::InvalidMagicNumber);
		if (header.Width < 1u)
			return TRAP::MakeUnexpected(PPMErrorCode::InvalidWidth);
		if (header.Height < 1u)
			return TRAP::MakeUnexpected(PPMErrorCode::InvalidHeight);
		if(header.MaxValue <= 0u || header.MaxValue > std::numeric_limits<u16>::max())
			return TRAP::MakeUnexpected(PPMErrorCode::InvalidMaxValue);

		return header;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u8>, PPMErrorCode> LoadPixelData24BPP(std::ifstream& file, const u32 width, const u32 height)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		std::vector<u8> pixelData(NumericCast<usize>(width) * height * 3u);

		if (!file.read(reinterpret_cast<char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size())))
			return TRAP::MakeUnexpected(PPMErrorCode::FailedToReadPixelData);

		return pixelData;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u16>, PPMErrorCode> LoadPixelData48BPP(std::ifstream& file, const u32 width, const u32 height)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		std::vector<u16> pixelData(NumericCast<usize>(width) * height * 3u);

		if(!file.read(reinterpret_cast<char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size() * sizeof(u16))))
			return TRAP::MakeUnexpected(PPMErrorCode::FailedToReadPixelData);

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

	const auto header = LoadHeader(file);
	if(!header)
	{
		TP_ERROR(Log::ImagePPMPrefix, PPMErrorCodeToString(header.Error()));
		TP_WARN(Log::ImagePPMPrefix, "Using default image!");
		return;
	}

	m_width = header->Width;
	m_height = header->Height;

	file.ignore(256, '\n'); //Skip ahead to the pixel data.

	if(header->MaxValue > 255u)
	{
		m_bitsPerPixel = 48u;

		const auto pixelData = LoadPixelData48BPP(file, m_width, m_height);
		if(!pixelData)
		{
			TP_ERROR(Log::ImagePPMPrefix, PPMErrorCodeToString(pixelData.Error()));
			TP_WARN(Log::ImagePPMPrefix, "Using default image!");
			return;
		}

		m_data2Byte = *pixelData;
	}
	else
	{
		m_bitsPerPixel = 24u;

		const auto pixelData = LoadPixelData24BPP(file, m_width, m_height);
		if(!pixelData)
		{
			TP_ERROR(Log::ImagePPMPrefix, PPMErrorCodeToString(pixelData.Error()));
			TP_WARN(Log::ImagePPMPrefix, "Using default image!");
			return;
		}

		m_data = *pixelData;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::PPMImage::Save(const Image& img, const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	//NOTE Only supports 24/32BPP RGB(A) Input

	if((img.GetBitsPerPixel() != 24u && img.GetBitsPerPixel() != 32u) || !img.IsLDR())
	{
		TP_ERROR(Log::ImagePPMPrefix, "Failed to save image: ", filepath);
		return;
	}

	std::ofstream file(filepath, std::ios::out | std::ios::binary);

	//PPM Header
	file << fmt::format("P6\n{}\n{}\n255\n", img.GetWidth(), img.GetHeight());

	//PPM Data
	if(img.GetColorFormat() == ColorFormat::RGBA)
	{
		const std::vector<u8> pixelData = TRAP::Utils::ConvertRGBAPixelDataToRGB<u8>(img.GetWidth(), img.GetHeight(), img.GetPixelData());
		file.write(reinterpret_cast<const char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size()));
	}
	else
	{
		const std::span pixelData = img.GetPixelData();
		file.write(reinterpret_cast<const char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size()));
	}
}
