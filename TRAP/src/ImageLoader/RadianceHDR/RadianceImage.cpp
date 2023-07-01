#include "TRAPPCH.h"
#include "RadianceImage.h"

#include "Application.h"
#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"

TRAP::INTERNAL::RadianceImage::RadianceImage(std::filesystem::path filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	m_filepath = std::move(filepath);
	m_isHDR = true;
	m_bitsPerPixel = 96;
	m_colorFormat = ColorFormat::RGB;

	TP_DEBUG(Log::ImageRadiancePrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImageRadiancePrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	if(!ContainsMagicNumber(file))
	{
		TP_ERROR(Log::ImageRadiancePrefix, "Couldn't find magic number \"#?\"!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	if(!ContainsSupportedFormat(file))
	{
		TP_ERROR(Log::ImageRadiancePrefix, "Unknown or unsupported file format!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	SkipUnusedLines(file);

	bool needXFlip = false, needYFlip = false, need90RotateCW = false, need90RotateCCW = false;
	const std::optional<TRAP::Math::Vec2ui> resolution = RetrieveImageResolution(file, needXFlip, needYFlip,
	                                                                             need90RotateCW, need90RotateCCW);
	if(!resolution)
	{
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	m_width = resolution->x;
	m_height = resolution->y;

	if(m_width == 0)
	{
		file.close();
		TP_ERROR(Log::ImageRadiancePrefix, "Invalid width ", m_width, "!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}
	if(m_height == 0)
	{

		file.close();
		TP_ERROR(Log::ImageRadiancePrefix, "Invalid height ", m_height, "!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	m_data.resize(NumericCast<std::size_t>(m_width) * m_height * std::to_underlying(m_colorFormat), 0.0f);
	uint64_t dataIndex = 0;

	std::vector<RGBE> scanline(m_width);

	//Convert image
	for(int64_t y = m_height - 1; y >= 0; y--)
	{
		if (!Decrunch(scanline, m_width, file))
		{
			m_data.clear();
			TP_ERROR(Log::ImageRadiancePrefix, "Decrunching failed!");
			TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
			return;
		}
		WorkOnRGBE(scanline, m_data, dataIndex);
		dataIndex += NumericCast<uint64_t>(m_width) * std::to_underlying(m_colorFormat);
	}

	file.close();

	if (needXFlip)
		m_data = FlipX(m_width, m_height, m_colorFormat, m_data.data());
	if (needYFlip)
		m_data = FlipY(m_width, m_height, m_colorFormat, m_data.data());

	if(need90RotateCW)
		m_data = Rotate90Clockwise(m_width, m_height, m_colorFormat, m_data.data());
	if(need90RotateCCW)
		m_data = Rotate90CounterClockwise(m_width, m_height, m_colorFormat, m_data.data());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::INTERNAL::RadianceImage::ConvertComponent(const int8_t exponent, const int32_t value)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const float v = NumericCast<float>(value) / 256.0f;
	const float d = Math::Pow(2.0f, NumericCast<float>(exponent));

	return v * d;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::RadianceImage::Decrunch(std::vector<RGBE>& scanline, const uint32_t length,
                                                           std::ifstream& file)
{
	if (length < MinEncodingLength || length > MaxEncodingLength)
		return OldDecrunch(scanline, 0u, length, file);

	int32_t i = NumericCast<int32_t>(file.get());
	if (i != 2)
	{
		file.seekg(-1, std::ios::cur);
		return OldDecrunch(scanline, 0u, length, file);
	}

	scanline[0u][G] = NumericCast<uint8_t>(file.get());
	scanline[0u][B] = NumericCast<uint8_t>(file.get());
	i = file.get();

	if (scanline[0u][G] != 2u || (scanline[0][B] & 128u) != 0u)
	{
		scanline[0u][R] = 2u;
		scanline[0u][E] = NumericCast<uint8_t>(i);
		return OldDecrunch(scanline, 1u, length - 1u, file);
	}

	// read each component
	for (i = 0; i < 4; i++)
	{
	    for (uint32_t j = 0; j < length; )
		{
			uint8_t code = NumericCast<uint8_t>(file.get());
			if (code > 128u) //RLE
			{
			    code &= 127u;
			    const uint8_t value = NumericCast<uint8_t>(file.get());
			    while ((code--) != 0u)
					scanline[j++][i] = value;
			}
			else //Dump
			{
			    while((code--) != 0u)
					scanline[j++][i] = NumericCast<uint8_t>(file.get());
			}
		}
    }

	return !file.eof();
}

//-------------------------------------------------------------------------------------------------------------------//

//Old RLE
[[nodiscard]] bool TRAP::INTERNAL::RadianceImage::OldDecrunch(std::vector<RGBE>& scanline, uint32_t scanlineIndex,
                                                              uint32_t length, std::ifstream& file)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	uint32_t rshift = 0u;

	while (length > 0u)
	{
		scanline[scanlineIndex][R] = NumericCast<uint8_t>(file.get());
		scanline[scanlineIndex][G] = NumericCast<uint8_t>(file.get());
		scanline[scanlineIndex][B] = NumericCast<uint8_t>(file.get());
		scanline[scanlineIndex][E] = NumericCast<uint8_t>(file.get());
		if (file.eof())
			return false;

		if (scanline[scanlineIndex][R] == 1u &&
			scanline[scanlineIndex][G] == 1u &&
			scanline[scanlineIndex][B] == 1u)
		{
			for (uint32_t i = scanline[scanlineIndex][E] << rshift; i > 0u; i--)
			{
				memcpy(&scanline[0][0], &scanline[-1][0], 4u * sizeof(uint8_t));
				scanlineIndex++;
				length--;
			}
			rshift += 8u;
		}
		else {
			scanlineIndex++;
			length--;
			rshift = 0u;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::RadianceImage::WorkOnRGBE(std::vector<RGBE>& scanline, std::vector<float>& data,
                                               uint64_t dataIndex)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	uint32_t length = m_width;
	uint32_t scanlineIndex = 0;

	while (length-- > 0u)
	{
		const int8_t exponent = NumericCast<int8_t>(scanline[scanlineIndex][E] - 128);
		data[0 + dataIndex] = ConvertComponent(exponent, scanline[scanlineIndex][R]);
		data[1 + dataIndex] = ConvertComponent(exponent, scanline[scanlineIndex][G]);
		data[2 + dataIndex] = ConvertComponent(exponent, scanline[scanlineIndex][B]);
		dataIndex += 3;
		scanlineIndex++;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::RadianceImage::ContainsMagicNumber(std::ifstream& file)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	//Magic Numbers: "#?RGBE" or "#?RADIANCE"
	//Some software uses #? and then its own name... so we have to just check for #?

	std::string tmp;
	std::getline(file, tmp);

	return tmp.find("#?") != std::string::npos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::RadianceImage::ContainsSupportedFormat(std::ifstream& file)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	//We only support the "32-bit_rle_rgbe" format

	std::string tmp{};
	while(!file.eof())
	{
		std::getline(file, tmp);
		if(tmp.find("FORMAT=32-bit_rle_rgbe") != std::string::npos)
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::RadianceImage::SkipUnusedLines(std::ifstream& file)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if(file.eof())
		return;

	std::string tmp{};
	do
	{
		std::getline(file, tmp);
	} while(!tmp.empty());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<TRAP::Math::Vec2ui> TRAP::INTERNAL::RadianceImage::RetrieveImageResolution(std::ifstream& file,
                                                                                                       bool& outNeedXFlip,
																									   bool& outNeedYFlip,
																									   bool& outNeedRotateClockwise,
																									   bool& outNeedRotateCounterClockwise)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	outNeedXFlip = false;
	outNeedYFlip = false;
	outNeedRotateClockwise = false;
	outNeedRotateCounterClockwise = false;

	std::string resStr{};
	std::getline(file, resStr);

	std::size_t yIndex = resStr.find("Y ");
	std::size_t xIndex = resStr.find("X ");

	if(yIndex == std::string::npos || xIndex == std::string::npos ||
	   NumericCast<int64_t>(yIndex - 1) < 0 || NumericCast<int64_t>(xIndex - 1) < 0)
	{
		TP_ERROR(Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
		return std::nullopt;
	}

	if(yIndex < xIndex) //Y is before X
	{
		if(resStr[yIndex - 1] == '-')
			outNeedYFlip = false;
		else if(resStr[yIndex - 1] == '+')
			outNeedYFlip = true;
		else
		{
			TP_ERROR(Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
			return std::nullopt;
		}

		if(resStr[xIndex - 1] == '-')
			outNeedXFlip = true;
		else if(resStr[xIndex - 1] == '+')
			outNeedXFlip = false;
		else
		{
			TP_ERROR(Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
			return std::nullopt;
		}
	}
	else //X is before Y
	{
		if(resStr[xIndex - 1] == '+' && resStr[yIndex - 1] == '+')
		{
			outNeedRotateCounterClockwise = true;
		}
		else if(resStr[xIndex - 1] == '-' && resStr[yIndex - 1] == '+')
		{
			outNeedRotateCounterClockwise = true;
			outNeedYFlip = true;
		}
		else if(resStr[xIndex - 1] == '-' && resStr[yIndex - 1] == '-')
		{
			outNeedRotateClockwise = true;
		}
		else if(resStr[xIndex - 1] == '+' && resStr[yIndex - 1] == '-')
		{
			outNeedRotateClockwise = true;
			outNeedYFlip = true;
		}
		else
		{
			TP_ERROR(Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
			return std::nullopt;
		}
	}

	yIndex += 2;
	xIndex += 2;

	std::size_t yEnd = yIndex;
	while(yEnd < resStr.size() && Utils::String::IsDigit(resStr[yEnd]))
		++yEnd;

	std::size_t xEnd = xIndex;
	while(xEnd < resStr.size() && Utils::String::IsDigit(resStr[xEnd]))
		++xEnd;

	try
	{
		return TRAP::Math::Vec2ui(std::stoul(std::string(resStr.begin() + NumericCast<std::ptrdiff_t>(xIndex), resStr.begin() + NumericCast<std::ptrdiff_t>(xEnd))),
		                          std::stoul(std::string(resStr.begin() + NumericCast<std::ptrdiff_t>(yIndex), resStr.begin() + NumericCast<std::ptrdiff_t>(yEnd))));
	}
	catch(...)
	{
		TP_ERROR(Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
		return std::nullopt;
	}
}
