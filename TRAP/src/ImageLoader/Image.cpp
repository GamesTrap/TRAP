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
#include "Utils/ImageUtils.h"

namespace
{
	constexpr std::array<std::string_view, 15> SupportedImageFormatSuffixes
	{
		".pgm", ".ppm", ".pnm", ".pam", ".pfm",
		".tga", ".icb", ".vda", ".vst",
		".bmp", ".dib",
		".png",
		".hdr", ".pic",
		".qoi"
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::filesystem::path& TRAP::Image::GetFilePath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromFile(const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	if(!IsSupportedImageFile(filepath))
	{
		TP_ERROR(Log::ImagePrefix, "Unsupported or unknown image ", filepath, "!");
		TP_WARN(Log::ImagePrefix, "Using default image!");
		return MakeScope<INTERNAL::CustomImage>(filepath, 32, 32, ColorFormat::RGBA, std::vector<u8>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
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
		return MakeScope<INTERNAL::CustomImage>(filepath, 32, 32, ColorFormat::RGBA, std::vector<u8>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
	}

	//Test for Errors
	if (result->GetPixelData().empty())
		result = MakeScope<INTERNAL::CustomImage>(filepath, 32, 32, ColorFormat::RGBA, std::vector<u8>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromMemory(u32 width, u32 height, ColorFormat format, const std::vector<u8>& pixelData)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	return MakeScope<INTERNAL::CustomImage>("", width, height, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromMemory(u32 width, u32 height, ColorFormat format, const std::vector<u16>& pixelData)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	return MakeScope<INTERNAL::CustomImage>("", width, height, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromMemory(u32 width, u32 height, ColorFormat format, const std::vector<f32>& pixelData)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	return MakeScope<INTERNAL::CustomImage>("", width, height, format, pixelData);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFallback()
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	return MakeScope<INTERNAL::CustomImage>("", 32, 32, ColorFormat::RGBA, std::vector<u8>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() });
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::Image(std::filesystem::path filepath)
	: m_filepath(std::move(filepath))
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::Image(std::filesystem::path filepath, u32 width, u32 height, ColorFormat format)
	: m_width(width), m_height(height), m_colorFormat(format), m_filepath(std::move(filepath))
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Image::IsSupportedImageFile(const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	const auto fileEnding = FileSystem::GetFileEnding(filepath);
	if(!fileEnding)
		return false;

	const std::string fileFormat = Utils::String::ToLower(*fileEnding);

	return std::ranges::contains(SupportedImageFormatSuffixes, fileFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::FlipX(const Image& img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	Scope<Image> result;

	if (img.IsHDR())
	{
		const std::span<const f32> data{reinterpret_cast<const f32*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(f32)};
		const std::vector<f32> flipped = TRAP::Utils::FlipPixelDataX(img.GetWidth(), img.GetHeight(), img.GetChannelsPerPixel(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), flipped);
	}
	else if (img.IsLDR() && img.GetBytesPerChannel() == 2)
	{
		const std::span<const u16> data{reinterpret_cast<const u16*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(u16)};
		const std::vector<u16> flipped = TRAP::Utils::FlipPixelDataX(img.GetWidth(), img.GetHeight(), img.GetChannelsPerPixel(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), flipped);
	}
	else /*if(img->IsLDR() && img->GetBytesPerChannel() == 1)*/
	{
		const std::vector<u8> flipped = TRAP::Utils::FlipPixelDataX(img.GetWidth(), img.GetHeight(), img.GetChannelsPerPixel(), img.GetPixelData());

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), flipped);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::FlipY(const Image& img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	Scope<Image> result;

	if (img.IsHDR())
	{
		const std::span<const f32> data{reinterpret_cast<const f32*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(f32)};
		const std::vector<f32> flipped = TRAP::Utils::FlipPixelDataY(img.GetWidth(), img.GetHeight(), img.GetChannelsPerPixel(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), flipped);
	}
	else if (img.IsLDR() && img.GetBytesPerChannel() == 2)
	{
		const std::span<const u16> data{reinterpret_cast<const u16*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(u16)};
		const std::vector<u16> flipped = TRAP::Utils::FlipPixelDataY(img.GetWidth(), img.GetHeight(), img.GetChannelsPerPixel(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), flipped);
	}
	else /*if(img.IsLDR() && img.GetBytesPerChannel() == 1)*/
	{
		const std::vector<u8> flipped = TRAP::Utils::FlipPixelDataY(img.GetWidth(), img.GetHeight(), img.GetChannelsPerPixel(),
		                                      img.GetPixelData());

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), flipped);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::Rotate90Clockwise(const Image& img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	Scope<Image> result;

	if (img.IsHDR())
	{
		const std::span<const f32> data{reinterpret_cast<const f32*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(f32)};
		const std::vector<f32> rotated = TRAP::Utils::RotatePixelData90Clockwise(img.GetWidth(), img.GetHeight(),
		                                                            img.GetChannelsPerPixel(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), rotated);
	}
	else if (img.IsLDR() && img.GetBytesPerChannel() == 2)
	{
		const std::span<const u16> data{reinterpret_cast<const u16*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(u16)};
		const std::vector<u16> rotated = TRAP::Utils::RotatePixelData90Clockwise(img.GetWidth(), img.GetHeight(),
		                                                            img.GetChannelsPerPixel(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), rotated);
	}
	else /*if(img.IsLDR() && img.GetBytesPerChannel() == 1)*/
	{
		const std::vector<u8> rotated = TRAP::Utils::RotatePixelData90Clockwise(img.GetWidth(), img.GetHeight(),
		                                                           img.GetChannelsPerPixel(), img.GetPixelData());

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), rotated);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::Rotate90CounterClockwise(const Image& img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	Scope<Image> result;

	if (img.IsHDR())
	{
		const std::span<const f32> data{reinterpret_cast<const f32*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(f32)};
		const std::vector<f32> rotated = TRAP::Utils::RotatePixelData90CounterClockwise(img.GetWidth(), img.GetHeight(),
		                                                                   img.GetChannelsPerPixel(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), rotated);
	}
	else if (img.IsLDR() && img.GetBytesPerChannel() == 2)
	{
		const std::span<const u16> data{reinterpret_cast<const u16*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(u16)};
		const std::vector<u16> rotated = TRAP::Utils::RotatePixelData90CounterClockwise(img.GetWidth(), img.GetHeight(),
		                                                                   img.GetChannelsPerPixel(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), rotated);
	}
	else /*if(img.IsLDR() && img.GetBytesPerChannel() == 1)*/
	{
		const std::vector<u8> rotated = TRAP::Utils::RotatePixelData90CounterClockwise(img.GetWidth(), img.GetHeight(),
		                                                                  img.GetChannelsPerPixel(), img.GetPixelData());

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), img.GetColorFormat(), rotated);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::ConvertRGBToRGBA(const Image& img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	Scope<Image> result;

	if(img.IsHDR())
	{
		const std::span<const f32> data{reinterpret_cast<const f32*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(f32)};
		const std::vector<f32> converted = TRAP::Utils::ConvertRGBPixelDataToRGBA(img.GetWidth(), img.GetHeight(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), ColorFormat::RGBA, converted);
	}
	else if (img.IsLDR() && img.GetBytesPerChannel() == 2)
	{
		const std::span<const u16> data{reinterpret_cast<const u16*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(u16)};
		const std::vector<u16> converted = TRAP::Utils::ConvertRGBPixelDataToRGBA(img.GetWidth(), img.GetHeight(), data);

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), ColorFormat::RGBA, converted);
	}
	else /*if(img->IsLDR() && img->GetBytesPerChannel() == 1)*/
	{
		const std::vector<u8> flipped = TRAP::Utils::ConvertRGBPixelDataToRGBA(img.GetWidth(), img.GetHeight(),
		                                                 img.GetPixelData());

		result = LoadFromMemory(img.GetWidth(), img.GetHeight(), ColorFormat::RGBA, flipped);
	}

	return result;
}
