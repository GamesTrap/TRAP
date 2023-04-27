#include "TRAPPCH.h"
#include "RadianceImage.h"

#include "Application.h"
#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"

TRAP::INTERNAL::RadianceImage::RadianceImage(std::filesystem::path filepath)
	: eMax(-127), eMin(127)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	m_filepath = std::move(filepath);
	m_isHDR = true;
	m_bitsPerPixel = 96;
	m_colorFormat = ColorFormat::RGB;

	TP_DEBUG(Log::ImageRadiancePrefix, "Loading image: \"",
	         m_filepath.u8string(), "\"");

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImageRadiancePrefix, "Couldn't open file path: ", m_filepath.u8string(), "!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	//Magic Number check
	std::string str;
	std::getline(file, str);
	//Magic Number = 23 3F 52 41 44 49 41 4E 43 45 0A == #?RADIANCE
	//Magic Number = 23 3F 52 47 42 45 == #?RGBE
	if(str.find("#?RADIANCE") == std::string::npos && str.find("#?RGBE") == std::string::npos)
	{
		file.close();
		TP_ERROR(Log::ImageRadiancePrefix, "Invalid magic number ", str, "!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	bool formatFound = false;
	do
	{
		std::getline(file, str);
		if (str.find("FORMAT=32-bit_rle_rgbe") != std::string::npos)
			formatFound = true;
	} while (!str.empty());

	if(!formatFound)
	{
		file.close();
		TP_ERROR(Log::ImageRadiancePrefix, "Invalid file format", str, "!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	const char signOne = NumericCast<char>(file.get());
	const char axisOne = NumericCast<char>(file.get());
	file >> m_width;
	file.ignore();
	const char signTwo = NumericCast<char>(file.get());
	const char axisTwo = NumericCast<char>(file.get());
	file >> m_height;
	file.ignore();

	bool needYFlip = false;
	bool needXFlip = false;
	if(axisOne == 'Y' && axisTwo == 'X')
	{
		if (signOne == '+' && signTwo != '+')
			needYFlip = true;
		else if (signTwo == '-' && signOne != '-')
			needXFlip = true;
	}
	else if(axisOne == 'X' && axisTwo == 'Y')
	{
		if (signOne == '-' && signTwo != '-')
			needXFlip = true;
		else if (signTwo == '+' && signOne != '+')
			needYFlip = true;
	}
	else
	{
		file.close();
		TP_ERROR(Log::ImageRadiancePrefix, "Invalid resolution ", signOne, axisOne, ' ',
		         signTwo, axisTwo, "!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

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

	m_data.resize(NumericCast<std::size_t>(m_width) * m_height * 3, 0.0f);
	uint32_t dataIndex = 0;

	std::vector<std::array<uint8_t, 4>> scanline;
	scanline.resize(m_width);

	//Convert image
	for(int32_t y = NumericCast<int32_t>(m_height) - 1; y >= 0; y--)
	{
		if (!Decrunch(scanline, m_width, file))
		{
			m_data.clear();
			file.close();
			TP_ERROR(Log::ImageRadiancePrefix, "Decrunching failed!");
			TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
			return;
		}
		WorkOnRGBE(scanline, m_data, dataIndex);
		dataIndex += m_width * 3;
	}

	file.close();

	if (needXFlip)
		m_data = FlipX(m_width, m_height, m_colorFormat, m_data.data());
	if (needYFlip)
		m_data = FlipY(m_width, m_height, m_colorFormat, m_data.data());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const void* TRAP::INTERNAL::RadianceImage::GetPixelData() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t TRAP::INTERNAL::RadianceImage::GetPixelDataSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_data.size() * sizeof(float);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::INTERNAL::RadianceImage::ConvertComponent(const int8_t exponent, const int32_t value)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (exponent == -128)
		return 0.0f;

	const float v = NumericCast<float>(value) / 256.0f;
	const float d = Math::Pow(2.0f, NumericCast<float>(exponent));

	return v * d;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::RadianceImage::Decrunch(std::vector<std::array<uint8_t, 4>>& scanline,
                                                           const uint32_t length,
                                                           std::ifstream& file)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if (length < MinEncodingLength || length > MaxEncodingLength)
		return OldDecrunch(scanline, 0, length, file);

	uint8_t i = NumericCast<uint8_t>(file.get());
	if(i != 2u)
	{
		file.seekg(-1, std::ifstream::cur);
		return OldDecrunch(scanline, 0, length, file);
	}

	scanline[0][G] = NumericCast<uint8_t>(file.get());
	scanline[0][B] = NumericCast<uint8_t>(file.get());
	i = NumericCast<uint8_t>(file.get());

	if(scanline[0][G] != 2 || ((scanline[0][B] & 128u) != 0u))
	{
		scanline[0][R] = 2;
		scanline[0][E] = i;
		return OldDecrunch(scanline, 1, length - 1, file);
	}

	//read each component
	for(i = 0u; i < 4u; i++)
	{
		for(uint32_t j = 0; j < length;)
		{
			uint8_t code = NumericCast<uint8_t>(file.get());
			if(code > 128) //RLE
			{
				code &= 127u;
				const uint8_t value = NumericCast<uint8_t>(file.get());
				while ((code--) != 0u)
					scanline[j++][i] = value;
			}
			else //Non-RLE
			{
				while ((code--) != 0u)
					scanline[j++][i] = NumericCast<uint8_t>(file.get());
			}
		}
	}

	return !file.eof();
}

//-------------------------------------------------------------------------------------------------------------------//

//Old RLE
[[nodiscard]] bool TRAP::INTERNAL::RadianceImage::OldDecrunch(std::vector<std::array<uint8_t, 4>>& scanline,
                                                              uint32_t scanlineIndex, uint32_t length, std::ifstream& file)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	uint32_t rshift = 0;

	while (length > 0)
	{
		scanline[0 + scanlineIndex][R] = NumericCast<uint8_t>(file.get());
		scanline[0 + scanlineIndex][G] = NumericCast<uint8_t>(file.get());
		scanline[0 + scanlineIndex][B] = NumericCast<uint8_t>(file.get());
		scanline[0 + scanlineIndex][E] = NumericCast<uint8_t>(file.get());
		if (file.eof())
			return false;

		if(scanline[0 + scanlineIndex][R] == 1 && scanline[0 + scanlineIndex][G] == 1 && scanline[0 + scanlineIndex][B] == 1)
		{
			for(int32_t i = scanline[0 + scanlineIndex][E] << rshift; i > 0; i--)
			{
				std::copy_n(scanline[NumericCast<std::size_t>(-1 + scanlineIndex)].data(), 4, scanline[0 + scanlineIndex].data());
				scanlineIndex++;
				length--;
			}
			rshift += 8u;
		}
		else
		{
			scanlineIndex++;
			length--;
			rshift = 0u;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::RadianceImage::WorkOnRGBE(std::vector<std::array<uint8_t, 4>>& scanline,
                                               std::vector<float>& data, uint32_t dataIndex)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	int32_t length = NumericCast<int32_t>(m_width);
	uint32_t scanlineIndex = 0;

	while(length-- > 0)
	{
		const int8_t exponent = NumericCast<int8_t>(scanline[0 + scanlineIndex][E] - 128);
		if (exponent > eMax)
			eMax = exponent;
		if (exponent != -128 && exponent < eMin)
			eMin = exponent;

		data[0 + dataIndex] = ConvertComponent(exponent, scanline[0 + scanlineIndex][R]);
		data[1 + dataIndex] = ConvertComponent(exponent, scanline[0 + scanlineIndex][G]);
		data[2 + dataIndex] = ConvertComponent(exponent, scanline[0 + scanlineIndex][B]);
		dataIndex += 3;
		scanlineIndex++;
	}
}