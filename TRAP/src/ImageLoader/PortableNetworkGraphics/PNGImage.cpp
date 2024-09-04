#include "TRAPPCH.h"
#include "PNGImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/UniqueResource.h"

#include <png.h>

namespace
{
	void CFileCloser(FILE* const file)
	{
		if(file != nullptr)
			fclose(file);
	};

	//-------------------------------------------------------------------------------------------------------------------//

	struct Header
	{
		u32 Width = 0u;
		u32 Height = 0u;
		TRAP::Image::ColorFormat Format = TRAP::Image::ColorFormat::RGBA;
		u32 BitsPerPixel = 0u;
	};

	[[nodiscard]] Header GetHeaderData(const png_struct& png, const png_info& info)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		Header header{};

		header.Width = png_get_image_width(&png, &info);
		header.Height = png_get_image_height(&png, &info);

		const u8 bitDepth = png_get_bit_depth(&png, &info);
		const u8 channels = png_get_channels(&png, &info);
		header.BitsPerPixel = bitDepth * channels;

		const u8 colorType = png_get_color_type(&png, &info);
		if(colorType == PNG_COLOR_TYPE_GRAY)
			header.Format = TRAP::Image::ColorFormat::GrayScale;
		else if(colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			header.Format = TRAP::Image::ColorFormat::GrayScaleAlpha;
		else if(colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_PALETTE)
			header.Format = TRAP::Image::ColorFormat::RGB;
		else if(colorType == PNG_COLOR_TYPE_RGBA)
			header.Format = TRAP::Image::ColorFormat::RGBA;

		if(png_get_valid(&png, &info, PNG_INFO_tRNS) != 0u)
		{
			if(header.Format == TRAP::Image::ColorFormat::GrayScale)
				header.Format = TRAP::Image::ColorFormat::GrayScaleAlpha;
			else if(header.Format == TRAP::Image::ColorFormat::RGB)
				header.Format = TRAP::Image::ColorFormat::RGBA;
		}

		return header;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename T>
	requires std::same_as<T, u8> || std::same_as<T, u16>
	[[nodiscard]] std::vector<T> LoadPixelData(png_struct& png, const png_info& info, const u32 height)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		const usize rowBytes = png_get_rowbytes(&png, &info);
		std::vector<u8*> rowPointers(height);

		std::vector<T> pixelData(height * rowBytes / sizeof(T));
		for(u32 i = 0; i < rowPointers.size(); ++i)
			rowPointers[i] = reinterpret_cast<u8*>(&pixelData[i * rowBytes / sizeof(T)]);

		png_read_image(&png, rowPointers.data());

		return pixelData;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void SetupImageForReading(png_struct& png, png_info& info)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		if(png_get_interlace_type(&png, &info) != PNG_INTERLACE_NONE) //Handle interlacing when needed
			png_set_interlace_handling(&png);

		const u8 bitDepth = png_get_bit_depth(&png, &info);
		const u8 colorType = png_get_color_type(&png, &info);
		if(colorType == PNG_COLOR_TYPE_PALETTE) //Indexed -> RGB
			png_set_palette_to_rgb(&png);
		if(colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8u)
			png_set_expand_gray_1_2_4_to_8(&png);

		if(png_get_valid(&png, &info, PNG_INFO_tRNS) != 0u)
			png_set_tRNS_to_alpha(&png);

		png_read_update_info(&png, &info);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::PNGImage::PNGImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	TP_DEBUG(Log::ImagePNGPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	TRAP::UniqueResource file = TRAP::MakeUniqueResourceChecked(fopen(m_filepath.string().c_str(), "rb"), nullptr, CFileCloser);
	if (file.Get() == nullptr)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return;
	}

	png_struct* png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if(png == nullptr)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Failed to create PNG read struct!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return;
	}

	png_info* info = png_create_info_struct(png);
	if(info == nullptr)
	{
		png_destroy_read_struct(&png, nullptr, nullptr);
		TP_ERROR(Log::ImagePNGPrefix, "Failed to create PNG info struct!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return;
	}

	if(setjmp(png_jmpbuf(png)))
	{
		png_destroy_read_struct(&png, &info, nullptr);
		TP_ERROR(Log::ImagePNGPrefix, "Error while decoding PNG detected!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return;
	}

	png_init_io(png, file.Get());
	png_read_info(png, info);

	const Header header = GetHeaderData(*png, *info);
	m_width = header.Width;
	m_height = header.Height;
	m_bitsPerPixel = header.BitsPerPixel;
	m_colorFormat = header.Format;

	SetupImageForReading(*png, *info);

	if(GetBitsPerChannel() == 16u)
		m_data2Byte = LoadPixelData<u16>(*png, *info, m_height);
	else
		m_data = LoadPixelData<u8>(*png, *info, m_height);

	png_destroy_read_struct(&png, &info, nullptr);
}
