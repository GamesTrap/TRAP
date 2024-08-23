#include "TRAPPCH.h"
#include "PFMImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/Expected.h"

namespace
{
	struct Header
	{
		std::string MagicNumber;
		u32 Width = 0;
		u32 Height = 0;
		f32 ByteOrder = -1.0f;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	enum class PFMErrorCode
	{
		InvalidMagicNumber,
		InvalidWidth,
		InvalidHeight,
		FailedToReadPixelData
	};

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::string PFMErrorCodeToString(const PFMErrorCode errorCode)
	{
		switch(errorCode)
		{
		case PFMErrorCode::InvalidMagicNumber:
			return "Invalid magic number detected, expected \"PF\", \"PF4\" or \"Pf\"!";

		case PFMErrorCode::InvalidWidth:
			return "Width is invalid!";

		case PFMErrorCode::InvalidHeight:
			return "Height is invalid!";

		case PFMErrorCode::FailedToReadPixelData:
			return "Failed to read pixel data!";
		}

		TRAP_ASSERT(false);
		return "";
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<Header, PFMErrorCode> LoadHeader(std::ifstream& file)
	{
		const auto SkipComments = [&file]()
		{
			std::string tmp{'#'};
			while(!tmp.empty() && tmp[0] == '#')
				file >> tmp;
			return tmp;
		};

		Header header{};
		header.MagicNumber = SkipComments();
		header.Width = std::stoi(SkipComments());
		header.Height = std::stoi(SkipComments());
		header.ByteOrder = std::stof(SkipComments());

		if (header.MagicNumber != "PF" && header.MagicNumber != "Pf" && header.MagicNumber != "PF4")
			return TRAP::MakeUnexpected(PFMErrorCode::InvalidMagicNumber);
		if (header.Width < 1)
			return TRAP::MakeUnexpected(PFMErrorCode::InvalidWidth);
		if (header.Height < 1)
			return TRAP::MakeUnexpected(PFMErrorCode::InvalidHeight);

		return header;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<f32>, PFMErrorCode> LoadPixelData(std::ifstream& file,
	                                                                           const u32 width, const u32 height,
																			   const u32 channels)
	{
		std::vector<f32> pixelData(NumericCast<usize>(width) * height * channels);

		if (!file.read(reinterpret_cast<char*>(pixelData.data()), NumericCast<std::streamsize>(pixelData.size() * sizeof(f32))))
			return TRAP::MakeUnexpected(PFMErrorCode::FailedToReadPixelData);

		return pixelData;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

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

	const auto header = LoadHeader(file);
	if(!header)
	{
		TP_ERROR(Log::ImagePFMPrefix, PFMErrorCodeToString(header.Error()));
		TP_WARN(Log::ImagePFMPrefix, "Using default image!");
		return;
	}

	m_width = header->Width;
	m_height = header->Height;

	file.ignore(256, '\n'); //Skip ahead to the pixel data

	if (header->MagicNumber == "PF")
	{
		m_colorFormat = ColorFormat::RGB;
		m_bitsPerPixel = 96;
	}
	else if (header->MagicNumber == "PF4")
	{
		m_colorFormat = ColorFormat::RGBA;
		m_bitsPerPixel = 128;
	}
	else if(header->MagicNumber == "Pf")
	{
		m_colorFormat = ColorFormat::GrayScale;
		m_bitsPerPixel = 32;
	}

	const auto pixelData = LoadPixelData(file, m_width, m_height, m_bitsPerPixel / 32u);
	if(!pixelData)
	{
		TP_ERROR(Log::ImagePFMPrefix, PFMErrorCodeToString(pixelData.Error()));
		TP_WARN(Log::ImagePFMPrefix, "Using default image!");
		return;
	}

	m_data = *pixelData;
}
