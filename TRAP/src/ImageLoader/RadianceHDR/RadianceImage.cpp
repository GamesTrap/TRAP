#include "TRAPPCH.h"
#include "RadianceImage.h"

#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"

namespace
{
	using RGBE = std::array<u8, 4>;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert exponent and value to floating point.
	/// As specified in the Radiance HDR specification.
	/// @param exponent Exponent.
	/// @param value Value.
	/// @return Floating point value.
	[[nodiscard]] constexpr f32 ConvertComponent(const i8 exponent, const i32 value)
	{
		const f32 v = NumericCast<f32>(value) / 256.0f;
		const f32 d = TRAP::Math::Pow(2.0f, NumericCast<f32>(exponent));

		return v * d;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	constexpr u32 MinEncodingLength = 8; //Minimum scanline length for encoding
	constexpr u32 MaxEncodingLength = 0x7FFF; //Maximum scanline length for encoding
	constexpr u32 R = 0;
	constexpr u32 G = 1;
	constexpr u32 B = 2;
	constexpr u32 E = 3;

	/// @brief Decode the given scanline.
	/// Used for old RLE encoding.
	/// @param scanline Storage for the decoded scanline.
	/// @param scanlineIndex Start index for decoding.
	/// @param length Scanline length.
	/// @param file Open Radiance HDR file.
	/// @return True if successful, false otherwise.
	[[nodiscard]] bool OldDecrunch(std::vector<RGBE>& scanline, u32 scanlineIndex, u32 length, std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

		u32 rshift = 0u;

		while (length > 0u)
		{
			scanline[scanlineIndex][R] = NumericCast<u8>(file.get());
			scanline[scanlineIndex][G] = NumericCast<u8>(file.get());
			scanline[scanlineIndex][B] = NumericCast<u8>(file.get());
			scanline[scanlineIndex][E] = NumericCast<u8>(file.get());
			if (file.eof())
				return false;

			if (scanline[scanlineIndex][R] == 1u &&
				scanline[scanlineIndex][G] == 1u &&
				scanline[scanlineIndex][B] == 1u)
			{
				for (u32 i = NumericCast<u32>(scanline[scanlineIndex][E] << rshift); i > 0u; i--)
				{
					memcpy(scanline[scanlineIndex].data(), scanline[scanlineIndex - 1].data(), 4u * sizeof(u8));
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

	/// @brief Decode the given scanline.
	/// Used for RLE encoding and uncompressed data.
	/// @param scanline Storage for the decoded scanline.
	/// @param length Scanline length.
	/// @param file Open Radiance HDR file.
	/// @return True if successful, false otherwise.
	[[nodiscard]] bool Decrunch(std::vector<RGBE>& scanline, const u32 length, std::ifstream& file)
	{
		if (length < MinEncodingLength || length > MaxEncodingLength)
			return OldDecrunch(scanline, 0u, length, file);

		i32 i = file.get();
		if (i != 2)
		{
			file.seekg(-1, std::ios::cur);
			return OldDecrunch(scanline, 0u, length, file);
		}

		scanline[0u][G] = NumericCast<u8>(file.get());
		scanline[0u][B] = NumericCast<u8>(file.get());
		i = file.get();

		if (scanline[0u][G] != 2u || (scanline[0][B] & 128u) != 0u)
		{
			scanline[0u][R] = 2u;
			scanline[0u][E] = NumericCast<u8>(i);
			return OldDecrunch(scanline, 1u, length - 1u, file);
		}

		// read each component
		for (i = 0; i < 4; i++)
		{
			for (u32 j = 0; j < length; )
			{
				u8 code = NumericCast<u8>(file.get());
				if (code > 128u) //RLE
				{
					code &= 127u;
					const u8 value = NumericCast<u8>(file.get());
					while ((code--) != 0u)
						scanline[j++][NumericCast<usize>(i)] = value;
				}
				else //Dump
				{
					while((code--) != 0u)
						scanline[j++][NumericCast<usize>(i)] = NumericCast<u8>(file.get());
				}
			}
		}

		return !file.eof();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Extract color values from the scanlines.
	/// @param scanline Scanlines.
	/// @param data Output storage for color data.
	/// @param dataIndex Index in the output storage to store data at.
	/// @param width Width in pixels of the image.
	constexpr void WorkOnRGBE(std::vector<RGBE>& scanline, std::vector<f32>& data, u64 dataIndex, const u32 width)
	{
		u32 length = width;
		u32 scanlineIndex = 0;

		while (length-- > 0u)
		{
			const i8 exponent = NumericCast<i8>(scanline[scanlineIndex][E] - 128);
			data[0 + dataIndex] = ConvertComponent(exponent, scanline[scanlineIndex][R]);
			data[1 + dataIndex] = ConvertComponent(exponent, scanline[scanlineIndex][G]);
			data[2 + dataIndex] = ConvertComponent(exponent, scanline[scanlineIndex][B]);
			dataIndex += 3;
			scanlineIndex++;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if file contains the magic number "#?".
	/// @param file File to check.
	/// @return True if magic number was found, false otherwise.
	[[nodiscard]] bool ContainsMagicNumber(std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

		//Magic Numbers: "#?RGBE" or "#?RADIANCE"
		//Some software uses #? and then its own name... so we have to just check for #?

		std::string tmp;
		std::getline(file, tmp);

		return TRAP::Utils::String::Contains(tmp, "#?");
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if file contains a valid/known format.
	/// Currently only "32-bit_rle_rgbe" is supported.
	/// @param file File to check.
	/// @return True if valid/known format was found, false otherwise.
	[[nodiscard]] bool ContainsSupportedFormat(std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

		//We only support the "32-bit_rle_rgbe" format

		std::string tmp{};
		while(!file.eof())
		{
			std::getline(file, tmp);
			if(TRAP::Utils::String::Contains(tmp, "FORMAT=32-bit_rle_rgbe"))
				return true;
		}

		return false;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Skip lines which are not used for decoding.
	/// @param file File to skip lines.
	void SkipUnusedLines(std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

		if(file.eof())
			return;

		std::string tmp{};
		do
		{
			std::getline(file, tmp);
		} while(!tmp.empty());
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the resolution data from file.
	/// @param file File to retrieve data from.
	/// @param outNeedXFlip True if image needs to be flipped on X axis.
	/// @param outNeedYFlip True if image needs to be flipped on Y axis.
	/// @param outNeedRotateClockwise True if image needs to be rotated 90 degrees clockwise.
	/// @param outNeedRotateCounterClockwise True if image needs to be rotated 90 degrees counter clockwise.
	/// @return Image resolution on success, empty optional otherwise.
	[[nodiscard]] std::optional<TRAP::Math::Vec2ui> RetrieveImageResolution(std::ifstream& file, bool& outNeedXFlip,
																			bool& outNeedYFlip,
																			bool& outNeedRotateClockwise,
																			bool& outNeedRotateCounterClockwise)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

		outNeedXFlip = false;
		outNeedYFlip = false;
		outNeedRotateClockwise = false;
		outNeedRotateCounterClockwise = false;

		std::string resStr{};
		std::getline(file, resStr);

		usize yIndex = resStr.find("Y ");
		usize xIndex = resStr.find("X ");

		if(yIndex == std::string::npos || xIndex == std::string::npos ||
		NumericCast<i64>(yIndex - 1) < 0 || NumericCast<i64>(xIndex - 1) < 0)
		{
			TP_ERROR(TRAP::Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
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
				TP_ERROR(TRAP::Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
				return std::nullopt;
			}

			if(resStr[xIndex - 1] == '-')
				outNeedXFlip = true;
			else if(resStr[xIndex - 1] == '+')
				outNeedXFlip = false;
			else
			{
				TP_ERROR(TRAP::Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
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
				TP_ERROR(TRAP::Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
				return std::nullopt;
			}
		}

		yIndex += 2;
		xIndex += 2;

		usize yEnd = yIndex;
		while(yEnd < resStr.size() && TRAP::Utils::String::IsDigit(resStr[yEnd]))
			++yEnd;

		usize xEnd = xIndex;
		while(xEnd < resStr.size() && TRAP::Utils::String::IsDigit(resStr[xEnd]))
			++xEnd;

		try
		{
			return TRAP::Math::Vec2ui(std::stoul(std::string(resStr.begin() + NumericCast<isize>(xIndex), resStr.begin() + NumericCast<isize>(xEnd))),
									  std::stoul(std::string(resStr.begin() + NumericCast<isize>(yIndex), resStr.begin() + NumericCast<isize>(yEnd))));
		}
		catch(...)
		{
			TP_ERROR(TRAP::Log::ImageRadiancePrefix, "Failed to retrieve image resolution!");
			return std::nullopt;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::RadianceImage::RadianceImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

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

	m_width = resolution->x();
	m_height = resolution->y();

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

	m_data.resize(NumericCast<usize>(m_width) * m_height * std::to_underlying(m_colorFormat), 0.0f);
	u64 dataIndex = 0;

	std::vector<RGBE> scanline(m_width);

	//Convert image
	for(i64 y = m_height - 1; y >= 0; y--)
	{
		if (!Decrunch(scanline, m_width, file))
		{
			m_data.clear();
			TP_ERROR(Log::ImageRadiancePrefix, "Decrunching failed!");
			TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
			return;
		}
		WorkOnRGBE(scanline, m_data, dataIndex, m_width);
		dataIndex += NumericCast<u64>(m_width) * std::to_underlying(m_colorFormat);
	}

	file.close();

	if (needXFlip)
		m_data = FlipX<f32>(m_width, m_height, m_colorFormat, m_data);
	if (needYFlip)
		m_data = FlipY<f32>(m_width, m_height, m_colorFormat, m_data);

	if(need90RotateCW)
		m_data = Rotate90Clockwise<f32>(m_width, m_height, m_colorFormat, m_data);
	if(need90RotateCCW)
		m_data = Rotate90CounterClockwise<f32>(m_width, m_height, m_colorFormat, m_data);
}
