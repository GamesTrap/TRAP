#include "TRAPPCH.h"
#include "Image.h"

#include "FileSystem/FileSystem.h"
#include "Utils/String/String.h"

#include "PortableMaps/PGMImage.h"
#include "PortableMaps/PPMImage.h"
#include "PortableMaps/PNMImage.h"
#include "PortableMaps/PAMImage.h"
#include "PortableMaps/PFMImage.h"
#include "TARGA/TGAImage.h"
#include "Bitmap/BMPImage.h"
#include "PortableNetworkGraphics/PNGImage.h"
#include "RadianceHDR/RadianceImage.h"
#include "QuiteOKImage/QOIImage.h"
#include "CustomImage.h"
#include "Embed.h"

TRAP::Image::Image() noexcept
	: m_width(0), m_height(0), m_isHDR(false), m_colorFormat(ColorFormat::NONE), m_bitsPerPixel(0)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Image::GetBitsPerPixel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Image::GetBytesPerPixel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Image::GetBitsPerChannel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_bitsPerPixel / ToUnderlying(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Image::GetBytesPerChannel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetBytesPerPixel() / ToUnderlying(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Image::GetWidth() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Image::GetHeight() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2ui TRAP::Image::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Math::Vec2ui{ m_width, m_height };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Image::HasAlphaChannel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_colorFormat == ColorFormat::GrayScaleAlpha || m_colorFormat == ColorFormat::RGBA;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Image::IsImageGrayScale() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_colorFormat == ColorFormat::GrayScale || m_colorFormat == ColorFormat::GrayScaleAlpha;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Image::IsImageColored() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_colorFormat == ColorFormat::RGB || m_colorFormat == ColorFormat::RGBA;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Image::IsHDR() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_isHDR;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Image::IsLDR() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return !m_isHDR;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::filesystem::path& TRAP::Image::GetFilePath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Image::ColorFormat TRAP::Image::GetColorFormat() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	return m_colorFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<uint8_t> TRAP::Image::ConvertBGR16ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	std::vector<uint8_t> data{};
	data.resize(NumericCast<std::size_t>(width) * height * 3);

	uint32_t index = 0;
	for (uint32_t i = 0; i < width * height * 2; i += 2)
	{
		data[index++] = (source[i + 1u] << 1u) & 0xF8u;
		data[index++] = ((source[i + 1u] << 6u) | (source[i] >> 2u)) & 0xF8u;
		data[index++] = (source[i] << 3u) & 0xF8u;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<uint8_t> TRAP::Image::ConvertBGR24ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	for (uint32_t i = 0; i < width * height * 3; i += 3)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<uint8_t> TRAP::Image::ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	for (uint32_t i = 0; i < width * height * 4; i += 4)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<uint8_t> TRAP::Image::DecodeBGRAMap(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	std::vector<uint8_t> data{};
	data.resize(NumericCast<std::size_t>(width) * height * channels);

	uint32_t index = 0;
	for (uint32_t i = 0; i < width * height; i++)
	{
		if (channels == 1)
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels];
		else if (channels == 2)
		{
			data[index++] = (colorMap[NumericCast<std::size_t>(source[i]) * channels + 1u] << 1u) & 0xF8u;
			data[index++] = ((colorMap[NumericCast<std::size_t>(source[i]) * channels + 1u] << 6u) | (colorMap[static_cast<std::size_t>(source[i]) * channels] >> 2u)) & 0xF8u;
			data[index++] = (colorMap[static_cast<std::size_t>(source[i]) * channels] << 3u) & 0xF8u;
		}
		else if (channels == 3)
		{
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 2];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 1];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 0];
		}
		else if (channels == 4)
		{
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 2];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 1];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 0];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 3];
		}
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromFile(const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if(!IsSupportedImageFile(filepath))
	{
		TP_ERROR(Log::ImagePrefix, "Unsupported or unknown image \"", filepath, "\"!");
		TP_WARN(Log::ImagePrefix, "Using default image!");
		return MakeScope<INTERNAL::CustomImage>(filepath, 32, 32, ColorFormat::RGBA, std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
	}

	const auto fileEnding = FileSystem::GetFileEnding(filepath);
	if(!fileEnding)
		return nullptr;

	const std::string fileFormat = Utils::String::ToLower(*fileEnding);

	Scope<Image> result;

	if (fileFormat == ".pgm")
		result = MakeScope<INTERNAL::PGMImage>(filepath);
	else if (fileFormat == ".ppm")
		result = MakeScope<INTERNAL::PPMImage>(filepath);
	else if (fileFormat == ".pnm")
		result = MakeScope<INTERNAL::PNMImage>(filepath);
	else if (fileFormat == ".pam")
		result = MakeScope<INTERNAL::PAMImage>(filepath);
	else if (fileFormat == ".pfm")
		result = MakeScope<INTERNAL::PFMImage>(filepath);
	else if (fileFormat == ".tga" || fileFormat == ".icb" || fileFormat == ".vda" || fileFormat == ".vst")
		result = MakeScope<INTERNAL::TGAImage>(filepath);
	else if (fileFormat == ".bmp" || fileFormat == ".dib")
		result = MakeScope<INTERNAL::BMPImage>(filepath);
	else if (fileFormat == ".png")
		result = MakeScope<INTERNAL::PNGImage>(filepath);
	else if (fileFormat == ".hdr" || fileFormat == ".pic")
		result = MakeScope<INTERNAL::RadianceImage>(filepath);
	else if (fileFormat == ".qoi")
		result = MakeScope<INTERNAL::QOIImage>(filepath);
	else //Shouldn't be reached, just in case
	{
		TP_ERROR(Log::ImagePrefix, "Unsupported or unknown image format ", fileFormat, "!");
		TP_WARN(Log::ImagePrefix, "Using default image!");
		return MakeScope<INTERNAL::CustomImage>(filepath, 32, 32, ColorFormat::RGBA, std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
	}

	//Test for Errors
	if (result->GetPixelDataSize() == 0 || result->GetColorFormat() == ColorFormat::NONE)
		result = MakeScope<INTERNAL::CustomImage>(filepath, 32, 32, ColorFormat::RGBA, std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format, const std::vector<uint8_t>& pixelData)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	return MakeScope<INTERNAL::CustomImage>("", width, height, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format, const std::vector<uint16_t>& pixelData)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	return MakeScope<INTERNAL::CustomImage>("", width, height, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format, const std::vector<float>& pixelData)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	return MakeScope<INTERNAL::CustomImage>("", width, height, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFallback()
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	return MakeScope<INTERNAL::CustomImage>("", 32, 32, ColorFormat::RGBA, std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Image::IsSupportedImageFile(const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	const auto fileEnding = FileSystem::GetFileEnding(filepath);
	if(!fileEnding)
		return false;

	const std::string fileFormat = Utils::String::ToLower(*fileEnding);

	return std::any_of(SupportedImageFormatSuffixes.cbegin(), SupportedImageFormatSuffixes.cend(), [fileFormat](const std::string_view suffix)
	{
		return fileFormat == suffix;
	});
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::FlipX(const Image* const img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if(img == nullptr)
		return nullptr;

	Scope<Image> result;

	if (img->IsHDR() && img->GetBytesPerChannel() == 4)
	{
		const std::vector<float> flipped = FlipX(img->GetWidth(), img->GetHeight(),
		                                         img->GetColorFormat(),
												 static_cast<const float*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
	}
	else if (img->IsLDR() && img->GetBytesPerChannel() == 2)
	{
		const std::vector<uint16_t> flipped = FlipX(img->GetWidth(), img->GetHeight(), img->GetColorFormat(),
		                                            static_cast<const uint16_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
	}
	else /*if(img->IsLDR() && img->GetBytesPerChannel() == 1)*/
	{
		const std::vector<uint8_t> flipped = FlipX(img->GetWidth(), img->GetHeight(), img->GetColorFormat(),
		                                           static_cast<const uint8_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::FlipY(const Image* const img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if (img == nullptr)
		return nullptr;

	Scope<Image> result;

	if (img->IsHDR() && img->GetBytesPerChannel() == 4)
	{
		const std::vector<float> flipped = FlipY(img->GetWidth(), img->GetHeight(), img->GetColorFormat(),
		                                         static_cast<const float*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
	}
	else if (img->IsLDR() && img->GetBytesPerChannel() == 2)
	{
		const std::vector<uint16_t> flipped = FlipY(img->GetWidth(), img->GetHeight(), img->GetColorFormat(),
		                                            static_cast<const uint16_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
	}
	else /*if(img->IsLDR() && img->GetBytesPerChannel() == 1)*/
	{
		const std::vector<uint8_t> flipped = FlipY(img->GetWidth(), img->GetHeight(), img->GetColorFormat(),
		                                           static_cast<const uint8_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::ConvertRGBToRGBA(const Image* const img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if(img == nullptr)
		return nullptr;

	Scope<Image> result;

	if(img->IsHDR() && img->GetBytesPerChannel() == 4)
	{
		const std::vector<float> converted = ConvertRGBToRGBA(img->GetWidth(), img->GetHeight(),
															  img->GetColorFormat(),
															  static_cast<const float*>(img->GetPixelData()));
		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), ColorFormat::RGBA, converted);
	}
	else if (img->IsLDR() && img->GetBytesPerChannel() == 2)
	{
		const std::vector<uint16_t> converted = ConvertRGBToRGBA(img->GetWidth(), img->GetHeight(),
		                                                         img->GetColorFormat(),
		                                                         static_cast<const uint16_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), ColorFormat::RGBA, converted);
	}
	else /*if(img->IsLDR() && img->GetBytesPerChannel() == 1)*/
	{
		const std::vector<uint8_t> flipped = ConvertRGBToRGBA(img->GetWidth(), img->GetHeight(),
		                                                      img->GetColorFormat(),
		                                                      static_cast<const uint8_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), ColorFormat::RGBA, flipped);
	}

	return result;
}