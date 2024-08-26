#include "TRAPPCH.h"
#include "RadianceImage.h"

#include "Utils/ImageUtils.h"
#include "Utils/String/String.h"
#include "Utils/Expected.h"
#include "FileSystem/FileSystem.h"

namespace
{
	using RGBE = std::array<u8, 4u>;

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

	constexpr u32 MinEncodingLength = 8u; //Minimum scanline length for encoding
	constexpr u32 MaxEncodingLength = 0x7FFFu; //Maximum scanline length for encoding
	constexpr u32 R = 0u;
	constexpr u32 G = 1u;
	constexpr u32 B = 2u;
	constexpr u32 E = 3u;

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
			file.read(reinterpret_cast<char*>(scanline[scanlineIndex].data()), NumericCast<std::streamsize>(scanline[scanlineIndex].size()));
			if (!file.good() || file.eof())
				return false;

			if (std::get<R>(scanline[scanlineIndex]) == 1u &&
				std::get<G>(scanline[scanlineIndex]) == 1u &&
				std::get<B>(scanline[scanlineIndex]) == 1u)
			{
				for (u32 i = NumericCast<u32>(std::get<E>(scanline[scanlineIndex]) << rshift); i > 0u; i--)
				{
					std::ranges::copy(scanline[scanlineIndex - 1], scanline[scanlineIndex].begin());
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

		std::get<G>(scanline[0u]) = NumericCast<u8>(file.get());
		std::get<B>(scanline[0u]) = NumericCast<u8>(file.get());
		i = file.get();

		if (std::get<G>(scanline[0u]) != 2u || (std::get<B>(scanline[0]) & 128u) != 0u)
		{
			std::get<R>(scanline[0u]) = 2u;
			std::get<E>(scanline[0u]) = NumericCast<u8>(i);
			return OldDecrunch(scanline, 1u, length - 1u, file);
		}

		// read each component
		for (i = 0; i < 4; i++)
		{
			for (u32 j = 0u; j < length; )
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
		u32 scanlineIndex = 0u;

		while (length-- > 0u)
		{
			const i8 exponent = NumericCast<i8>(std::get<E>(scanline[scanlineIndex]) - 128);
			data[R + dataIndex] = ConvertComponent(exponent, std::get<R>(scanline[scanlineIndex]));
			data[G + dataIndex] = ConvertComponent(exponent, std::get<G>(scanline[scanlineIndex]));
			data[B + dataIndex] = ConvertComponent(exponent, std::get<B>(scanline[scanlineIndex]));
			dataIndex += 3u;
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
		} while(!tmp.empty() && file.good());
	}

	//-------------------------------------------------------------------------------------------------------------------//

	struct ImageInfo
	{
		u32 Width = 0;
		u32 Height = 0;
		bool NeedsXFlip = false;
		bool NeedsYFlip = false;
		bool NeedsRotateClockwise = false;
		bool NeedsRotateCounterClockwise = false;
	};

	/// @brief Retrieve information about the image.
	/// @param file File to retrieve data from.
	/// @return Image information on success, error otherwise.
	[[nodiscard]] TRAP::Expected<ImageInfo, std::string> RetrieveImageInfo(std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

		ImageInfo imgInfo{};

		std::string resStr{};
		std::getline(file, resStr);

		usize yIndex = resStr.find("Y ");
		usize xIndex = resStr.find("X ");

		if(yIndex == std::string::npos || xIndex == std::string::npos ||
		   NumericCast<i64>(yIndex) - 1 < 0 || NumericCast<i64>(xIndex) - 1 < 0)
		{
			return TRAP::MakeUnexpected("Failed to retrieve image resolution!");
		}

		if(yIndex < xIndex) //Y is before X
		{
			if(resStr[yIndex - 1u] == '-')
				imgInfo.NeedsYFlip = false;
			else if(resStr[yIndex - 1u] == '+')
				imgInfo.NeedsYFlip = true;
			else
				return TRAP::MakeUnexpected("Failed to retrieve image resolution!");

			if(resStr[xIndex - 1u] == '-')
				imgInfo.NeedsXFlip = true;
			else if(resStr[xIndex - 1u] == '+')
				imgInfo.NeedsXFlip = false;
			else
				return TRAP::MakeUnexpected("Failed to retrieve image resolution!");
		}
		else //X is before Y
		{
			if((resStr[xIndex - 1u] == '+' && resStr[yIndex - 1u] == '+') ||
			   (resStr[xIndex - 1u] == '-' && resStr[yIndex - 1u] == '-'))
			{
				imgInfo.NeedsRotateClockwise = true;
			}
			else if((resStr[xIndex - 1u] == '-' && resStr[yIndex - 1u] == '+') ||
			        (resStr[xIndex - 1u] == '+' && resStr[yIndex - 1u] == '-'))
			{
				imgInfo.NeedsRotateCounterClockwise = true;
				imgInfo.NeedsYFlip = true;
			}
			else
				return TRAP::MakeUnexpected("Failed to retrieve image resolution!");
		}

		yIndex += 2u;
		xIndex += 2u;

		usize yEnd = yIndex;
		while(yEnd < resStr.size() && TRAP::Utils::String::IsDigit(resStr[yEnd]))
			++yEnd;

		usize xEnd = xIndex;
		while(xEnd < resStr.size() && TRAP::Utils::String::IsDigit(resStr[xEnd]))
			++xEnd;

		try
		{
			imgInfo.Width = NumericCast<u32>(std::stoul(std::string(resStr.begin() + NumericCast<isize>(xIndex), resStr.begin() + NumericCast<isize>(xEnd))));
			imgInfo.Height = NumericCast<u32>(std::stoul(std::string(resStr.begin() + NumericCast<isize>(yIndex), resStr.begin() + NumericCast<isize>(yEnd))));
		}
		catch(...)
		{
			return TRAP::MakeUnexpected("Failed to retrieve image resolution!");
		}

		return imgInfo;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::RadianceImage::RadianceImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	m_isHDR = true;
	m_bitsPerPixel = 96u;
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

	const auto imageInfo = RetrieveImageInfo(file);
	if(!imageInfo)
	{
		TP_ERROR(Log::ImageRadiancePrefix, imageInfo.Error());
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	m_width = imageInfo->Width;
	m_height = imageInfo->Height;

	if(m_width == 0u)
	{
		TP_ERROR(Log::ImageRadiancePrefix, "Invalid width ", m_width, "!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}
	if(m_height == 0u)
	{
		TP_ERROR(Log::ImageRadiancePrefix, "Invalid height ", m_height, "!");
		TP_WARN(Log::ImageRadiancePrefix, "Using default image!");
		return;
	}

	m_data.resize(NumericCast<usize>(m_width) * m_height * GetChannelsPerPixel(), 0.0f);
	u64 dataIndex = 0u;

	std::vector<RGBE> scanline(m_height);

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
		dataIndex += NumericCast<u64>(m_width) * GetChannelsPerPixel();
	}

	file.close();

	if (imageInfo->NeedsXFlip)
		m_data = TRAP::Utils::FlipPixelDataX<f32>(m_width, m_height, GetChannelsPerPixel(), m_data);
	if (imageInfo->NeedsYFlip)
		m_data = TRAP::Utils::FlipPixelDataY<f32>(m_width, m_height, GetChannelsPerPixel(), m_data);

	if(imageInfo->NeedsRotateClockwise)
		m_data = TRAP::Utils::RotatePixelData90Clockwise<f32>(m_width, m_height, GetChannelsPerPixel(), m_data);
	if(imageInfo->NeedsRotateCounterClockwise)
		m_data = TRAP::Utils::RotatePixelData90CounterClockwise<f32>(m_width, m_height, GetChannelsPerPixel(), m_data);
}
