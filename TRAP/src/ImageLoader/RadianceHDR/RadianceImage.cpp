#include "TRAPPCH.h"
#include "RadianceImage.h"

#include "Application.h"
#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"

TRAP::INTERNAL::RadianceImage::RadianceImage(std::filesystem::path filepath)
	: eMax(-127), eMin(127)
{
	ZoneScopedC(tracy::Color::Green);

	m_filepath = std::move(filepath);
	m_isHDR = true;
	m_bitsPerPixel = 96;
	m_colorFormat = ColorFormat::RGB;

	TP_DEBUG(Log::ImageRadiancePrefix, "Loading image: \"",
	         m_filepath.u8string(), "\"");

	if (!FileSystem::FileOrFolderExists(m_filepath))
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

	const char signOne = static_cast<char>(file.get());
	const char axisOne = static_cast<char>(file.get());
	file >> m_width;
	file.ignore();
	const char signTwo = static_cast<char>(file.get());
	const char axisTwo = static_cast<char>(file.get());
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

	m_data.resize(static_cast<std::size_t>(m_width) * m_height * 3, 0.0f);
	uint32_t dataIndex = 0;

	std::vector<std::array<uint8_t, 4>> scanline;
	scanline.resize(m_width);

	//Convert image
	for(int32_t y = static_cast<int32_t>(m_height) - 1; y >= 0; y--)
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

const void* TRAP::INTERNAL::RadianceImage::GetPixelData() const
{
	ZoneScopedC(tracy::Color::Green);

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::INTERNAL::RadianceImage::GetPixelDataSize() const
{
	ZoneScopedC(tracy::Color::Green);

	return m_data.size() * sizeof(float);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::INTERNAL::RadianceImage::ConvertComponent(const int8_t exponent, const int32_t value)
{
	ZoneScopedC(tracy::Color::Green);

	if (exponent == -128)
		return 0.0f;

	const float v = static_cast<float>(value) / 256.0f;
	const float d = Math::Pow(2.0f, static_cast<float>(exponent));

	return v * d;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::RadianceImage::Decrunch(std::vector<std::array<uint8_t, 4>>& scanline,
                                             const uint32_t length,
                                             std::ifstream& file)
{
	ZoneScopedC(tracy::Color::Green);

	if (length < MinEncodingLength || length > MaxEncodingLength)
		return OldDecrunch(scanline, 0, length, file);

	int32_t i = static_cast<int32_t>(file.get());
	if(i != 2)
	{
		file.seekg(-1, std::ifstream::cur);
		return OldDecrunch(scanline, 0, length, file);
	}

	scanline[0][G] = static_cast<uint8_t>(file.get());
	scanline[0][B] = static_cast<uint8_t>(file.get());
	i = static_cast<int32_t>(file.get());

	if(scanline[0][G] != 2 || scanline[0][B] & 128)
	{
		scanline[0][R] = 2;
		scanline[0][E] = static_cast<uint8_t>(i);
		return OldDecrunch(scanline, 1, length - 1, file);
	}

	//read each component
	for(i = 0; i < 4; i++)
	{
		for(uint32_t j = 0; j < length;)
		{
			uint8_t code = static_cast<uint8_t>(file.get());
			if(code > 128) //RLE
			{
				code &= 127;
				const uint8_t value = static_cast<uint8_t>(file.get());
				while (code--)
					scanline[j++][i] = value;
			}
			else //Non-RLE
			{
				while (code--)
					scanline[j++][i] = static_cast<uint8_t>(file.get());
			}
		}
	}

	return !file.eof();
}

//-------------------------------------------------------------------------------------------------------------------//

//Old RLE
bool TRAP::INTERNAL::RadianceImage::OldDecrunch(std::vector<std::array<uint8_t, 4>>& scanline,
                                                uint32_t scanlineIndex, uint32_t length, std::ifstream& file)
{
	ZoneScopedC(tracy::Color::Green);

	int32_t rshift = 0;

	while (length > 0)
	{
		scanline[0 + scanlineIndex][R] = static_cast<uint8_t>(file.get());
		scanline[0 + scanlineIndex][G] = static_cast<uint8_t>(file.get());
		scanline[0 + scanlineIndex][B] = static_cast<uint8_t>(file.get());
		scanline[0 + scanlineIndex][E] = static_cast<uint8_t>(file.get());
		if (file.eof())
			return false;

		if(scanline[0 + scanlineIndex][R] == 1 && scanline[0 + scanlineIndex][G] == 1 && scanline[0 + scanlineIndex][B] == 1)
		{
			for(int32_t i = scanline[0 + scanlineIndex][E] << rshift; i > 0; i--)
			{
				std::copy_n(&scanline[-1 + scanlineIndex][0], 4, &scanline[0 + scanlineIndex][0]);
				scanlineIndex++;
				length--;
			}
			rshift += 8;
		}
		else
		{
			scanlineIndex++;
			length--;
			rshift = 0;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::RadianceImage::WorkOnRGBE(std::vector<std::array<uint8_t, 4>>& scanline,
                                               std::vector<float>& data, uint32_t dataIndex)
{
	ZoneScopedC(tracy::Color::Green);

	int32_t length = static_cast<int32_t>(m_width);
	uint32_t scanlineIndex = 0;

	while(length-- > 0)
	{
		const int8_t exponent = static_cast<int8_t>(scanline[0 + scanlineIndex][E] - 128);
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