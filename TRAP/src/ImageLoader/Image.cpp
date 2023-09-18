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

[[nodiscard]] const std::filesystem::path& TRAP::Image::GetFilePath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::LoadFromFile(const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if(!IsSupportedImageFile(filepath))
	{
		TP_ERROR(Log::ImagePrefix, "Unsupported or unknown image ", filepath, "!");
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

TRAP::Image::Image(std::filesystem::path filepath)
	: m_filepath(std::move(filepath))
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::Image(std::filesystem::path filepath, uint32_t width, uint32_t height, ColorFormat format)
	: m_width(width), m_height(height), m_colorFormat(format), m_filepath(std::move(filepath))
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Image::IsSupportedImageFile(const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	const auto fileEnding = FileSystem::GetFileEnding(filepath);
	if(!fileEnding)
		return false;

	const std::string fileFormat = Utils::String::ToLower(*fileEnding);

	return std::ranges::any_of(SupportedImageFormatSuffixes, [&fileFormat](const std::string_view suffix)
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

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::Rotate90Clockwise(const Image* const img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if (img == nullptr)
		return nullptr;

	Scope<Image> result;

	if (img->IsHDR() && img->GetBytesPerChannel() == 4)
	{
		const std::vector<float> rotated = Rotate90Clockwise(img->GetWidth(), img->GetHeight(),
		                                                     img->GetColorFormat(),
		                                                     static_cast<const float*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}
	else if (img->IsLDR() && img->GetBytesPerChannel() == 2)
	{
		const std::vector<uint16_t> rotated = Rotate90Clockwise(img->GetWidth(), img->GetHeight(),
		                                                        img->GetColorFormat(),
		                                                        static_cast<const uint16_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}
	else /*if(img->IsLDR() && img->GetBytesPerChannel() == 1)*/
	{
		const std::vector<uint8_t> rotated = Rotate90Clockwise(img->GetWidth(), img->GetHeight(),
		                                                       img->GetColorFormat(),
		                                                       static_cast<const uint8_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Image::Rotate90CounterClockwise(const Image* const img)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if (img == nullptr)
		return nullptr;

	Scope<Image> result;

	if (img->IsHDR() && img->GetBytesPerChannel() == 4)
	{
		const std::vector<float> rotated = Rotate90CounterClockwise(img->GetWidth(), img->GetHeight(),
		                                                            img->GetColorFormat(),
		                                                            static_cast<const float*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}
	else if (img->IsLDR() && img->GetBytesPerChannel() == 2)
	{
		const std::vector<uint16_t> rotated = Rotate90CounterClockwise(img->GetWidth(), img->GetHeight(),
		                                                               img->GetColorFormat(),
		                                                               static_cast<const uint16_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}
	else /*if(img->IsLDR() && img->GetBytesPerChannel() == 1)*/
	{
		const std::vector<uint8_t> rotated = Rotate90CounterClockwise(img->GetWidth(), img->GetHeight(),
		                                                              img->GetColorFormat(),
																	  static_cast<const uint8_t*>(img->GetPixelData()));

		result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
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