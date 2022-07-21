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

const std::array<std::string, 15> TRAP::Image::SupportedImageFormatSuffixes
{
	".pgm", ".ppm", ".pnm", ".pam", ".pfm",
	".tga", ".icb", ".vda", ".vst",
	".bmp", ".dib",
	".png",
	".hdr", ".pic",
	".qoi"
};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::Image()
	: m_width(0), m_height(0), m_isHDR(false), m_colorFormat(ColorFormat::NONE), m_bitsPerPixel(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetBitsPerChannel() const
{
	return m_bitsPerPixel / static_cast<uint32_t>(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetBytesPerChannel() const
{
	return GetBytesPerPixel() / static_cast<uint32_t>(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Image::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2ui TRAP::Image::GetSize() const
{
	return Math::Vec2ui{ m_width, m_height };
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::HasAlphaChannel() const
{
	return m_colorFormat == ColorFormat::GrayScaleAlpha || m_colorFormat == ColorFormat::RGBA;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsImageGrayScale() const
{
	return m_colorFormat == ColorFormat::GrayScale || m_colorFormat == ColorFormat::GrayScaleAlpha;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsImageColored() const
{
	return m_colorFormat == ColorFormat::RGB || m_colorFormat == ColorFormat::RGBA;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsHDR() const
{
	return m_isHDR;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsLDR() const
{
	return !m_isHDR;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::filesystem::path& TRAP::Image::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::ColorFormat TRAP::Image::GetColorFormat() const
{
	return m_colorFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::ConvertBGR16ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	std::vector<uint8_t> data{};
	data.resize(static_cast<std::size_t>(width) * height * 3);

	uint32_t index = 0;
	for (uint32_t i = 0; i < width * height * 2; i += 2)
	{
		data[index++] = (source[i + 1] << 1) & 0xF8;
		data[index++] = ((source[i + 1] << 6) | (source[i] >> 2)) & 0xF8;
		data[index++] = (source[i] << 3) & 0xF8;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::ConvertBGR24ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	for (uint32_t i = 0; i < width * height * 3; i += 3)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	for (uint32_t i = 0; i < width * height * 4; i += 4)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::DecodeBGRAMap(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
{
	TP_PROFILE_FUNCTION();

	std::vector<uint8_t> data{};
	data.resize(static_cast<std::size_t>(width) * height * channels);

	uint32_t index = 0;
	for (uint32_t i = 0; i < width * height; i++)
	{
		if (channels == 1)
			data[index++] = colorMap[static_cast<std::size_t>(source[i]) * channels];
		else if (channels == 2)
		{
			data[index++] = (colorMap[source[i] * channels + 1] << 1) & 0xF8;
			data[index++] = ((colorMap[source[i] * channels + 1] << 6) | (colorMap[static_cast<std::size_t>(source[i]) * channels] >> 2)) & 0xF8;
			data[index++] = (colorMap[static_cast<std::size_t>(source[i]) * channels] << 3) & 0xF8;
		}
		else if (channels == 3)
		{
			data[index++] = colorMap[source[i] * channels + 2];
			data[index++] = colorMap[source[i] * channels + 1];
			data[index++] = colorMap[source[i] * channels + 0];
		}
		else if (channels == 4)
		{
			data[index++] = colorMap[source[i] * channels + 2];
			data[index++] = colorMap[source[i] * channels + 1];
			data[index++] = colorMap[source[i] * channels + 0];
			data[index++] = colorMap[source[i] * channels + 3];
		}
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromFile(const std::filesystem::path& filepath)
{
	TP_PROFILE_FUNCTION();

	if(!IsSupportedImageFile(filepath))
	{
		TP_ERROR(Log::ImagePrefix, "Unsupported or unknown image \"", filepath, "\"!");
		TP_WARN(Log::ImagePrefix, "Using default image!");
		return MakeScope<INTERNAL::CustomImage>(filepath, 32, 32, ColorFormat::RGBA, std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
	}

	const auto fileEnding = FileSystem::GetFileEnding(filepath);
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

TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format, const std::vector<uint8_t>& pixelData)
{
	TP_PROFILE_FUNCTION();

	return MakeScope<INTERNAL::CustomImage>("", width, height, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format, const std::vector<uint16_t>& pixelData)
{
	TP_PROFILE_FUNCTION();

	return MakeScope<INTERNAL::CustomImage>("", width, height, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format, const std::vector<float>& pixelData)
{
	TP_PROFILE_FUNCTION();

	return MakeScope<INTERNAL::CustomImage>("", width, height, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Image::LoadFallback()
{
	TP_PROFILE_FUNCTION();

	return MakeScope<INTERNAL::CustomImage>("", 32, 32, ColorFormat::RGBA, std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Image::IsSupportedImageFile(const std::filesystem::path& filepath)
{
	TP_PROFILE_FUNCTION();

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

TRAP::Scope<TRAP::Image> TRAP::Image::FlipX(const Image* const img)
{
	if(!img)
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

TRAP::Scope<TRAP::Image> TRAP::Image::FlipY(const Image* const img)
{
	if (!img)
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

TRAP::Scope<TRAP::Image> TRAP::Image::ConvertRGBToRGBA(const Image* const img)
{
	if(!img)
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