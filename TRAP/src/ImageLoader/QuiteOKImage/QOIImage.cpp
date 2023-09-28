/*
Copyright (c) 2021, Dominic Szablewski - https://phoboslab.org
SPDX-License-Identifier: MIT


QOI - The "Quite OK Image" format for fast, lossless image compression

Modified by Jan "GamesTrap" Schuerkamp
*/

#include "TRAPPCH.h"
#include "QOIImage.h"

#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"
#include "ImageLoader/Image.h"

constexpr std::array<uint8_t, 8> EndMarker{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::QOIImage::QOIImage(std::filesystem::path filepath)
    : Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	TP_DEBUG(Log::ImageQOIPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImageQOIPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImageQOIPrefix, "Using default image!");
		return;
	}

    const auto size = FileSystem::GetSize(m_filepath);
    std::size_t fileSize = 0;
    if(size)
        fileSize = *size;
    else //Fallback
    {
        file.seekg(0, std::ios::end);
        fileSize = static_cast<std::size_t>(file.tellg());
        file.seekg(0);
    }

    if(fileSize < sizeof(Header) + EndMarker.size())
    {
        TP_ERROR(Log::ImageQOIPrefix, "File size is too small: ", m_filepath, "!");
        TP_WARN(Log::ImageQOIPrefix, "Using default image!");
        return;
    }

    Header header{};
    file.read(header.MagicNumber.data(), NumericCast<uint32_t>(header.MagicNumber.size()));
    file.read(reinterpret_cast<char*>(&header.Width), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&header.Height), sizeof(uint32_t));
    file >> header.Channels >> header.ColorSpace;

    //Magic number must be "qoif"
    if(header.MagicNumber != "qoif")
    {
        TP_ERROR(Log::ImageQOIPrefix, "Invalid magic number: ", header.MagicNumber.data(), "!");
        TP_WARN(Log::ImageQOIPrefix, "Using default image!");
        return;
    }

    //Convert to machines endian
    if constexpr (Utils::GetEndian() != Utils::Endian::Big)
    {
        Utils::Memory::SwapBytes(header.Width);
        Utils::Memory::SwapBytes(header.Height);
    }

    //Validation checks
	if (header.Width < 1)
	{
		file.close();
		TP_ERROR(Log::ImageQOIPrefix, "Width is < 1 (", header.Width, ")!");
		TP_WARN(Log::ImageQOIPrefix, "Using default image!");
		return;
	}
	if (header.Height < 1)
	{
		file.close();
		TP_ERROR(Log::ImageQOIPrefix, "Height is < 1 (", header.Height, ")!");
		TP_WARN(Log::ImageQOIPrefix, "Using default image!");
		return;
	}
    if(header.Channels != 3 && header.Channels != 4)
    {
		file.close();
		TP_ERROR(Log::ImageQOIPrefix, "Invalid channel count, must be 3 or 4 (", header.Channels, ")!");
		TP_WARN(Log::ImageQOIPrefix, "Using default image!");
		return;
    }
    if(header.ColorSpace != 0 && header.ColorSpace != 1)
    {
		file.close();
		TP_ERROR(Log::ImageQOIPrefix, "Invalid color space, must be 0 (sRGB) or 1 (Linear) (", header.Channels, ")!");
		TP_WARN(Log::ImageQOIPrefix, "Using default image!");
		return;
    }

	m_width = header.Width;
	m_height = header.Height;

    m_data.resize(NumericCast<std::size_t>(header.Width) * header.Height * header.Channels);
    m_bitsPerPixel = header.Channels * 8;
    m_colorFormat = header.Channels == 3 ? ColorFormat::RGB : ColorFormat::RGBA;

    //TODO Treat as sRGB
    // if(header.ColorSpace == 0)
    // {

    // }
    // else
    // {

    // }

    DecodeImage(file, fileSize);

    file.close();
}

//-------------------------------------------------------------------------------------------------------------------//

struct Pixel
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t Alpha;
};

constexpr uint8_t QOI_OP_INDEX = 0x00;
constexpr uint8_t QOI_OP_DIFF  = 0x40;
constexpr uint8_t QOI_OP_LUMA  = 0x80;
constexpr uint8_t QOI_OP_RUN   = 0xC0;
constexpr uint8_t QOI_OP_RGB   = 0xFE;
constexpr uint8_t QOI_OP_RGBA  = 0xFF;

constexpr uint8_t QOI_MASK_2   = 0xC0;

[[nodiscard]] constexpr uint32_t QOI_COLOR_HASH(const Pixel& p) noexcept
{
    return p.Red * 3 + p.Green * 5 + p.Blue * 7 + p.Alpha * 11;
}

void TRAP::INTERNAL::QOIImage::DecodeImage(std::ifstream& file, const std::size_t& fileSize)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

    Pixel prevPixel{0, 0, 0, 255};
    std::array<Pixel, 64> prevPixels{};

    const std::size_t size = NumericCast<std::size_t>(m_width) * m_height * (m_bitsPerPixel / 8);
    std::size_t index = sizeof(Header);

    for(std::size_t pixelIndex = 0; pixelIndex < size;)
    {
        if(index < (fileSize - EndMarker.size()))
        {
            const uint8_t tag = NumericCast<uint8_t>(file.get());

            if(tag == QOI_OP_RGB)
            {
                prevPixel.Red = NumericCast<uint8_t>(file.get());
                prevPixel.Green = NumericCast<uint8_t>(file.get());
                prevPixel.Blue = NumericCast<uint8_t>(file.get());
            }
            else if(tag == QOI_OP_RGBA)
            {
                prevPixel.Red = NumericCast<uint8_t>(file.get());
                prevPixel.Green = NumericCast<uint8_t>(file.get());
                prevPixel.Blue = NumericCast<uint8_t>(file.get());
                prevPixel.Alpha = NumericCast<uint8_t>(file.get());
            }
            else
            {
                if((tag & QOI_MASK_2) == QOI_OP_INDEX)
                    prevPixel = prevPixels[tag & 0x3F];
                else if((tag & QOI_MASK_2) == QOI_OP_DIFF)
                {
                    prevPixel.Red   += NumericCast<uint8_t>((((tag & 0x30u) >> 4u) & 0x03u) - 2);
                    prevPixel.Green += NumericCast<uint8_t>((((tag & 0x0Cu) >> 2u) & 0x03u) - 2);
                    prevPixel.Blue  += NumericCast<uint8_t>((((tag & 0x03u) >> 0u) & 0x03u) - 2);
                }
                else if((tag & QOI_MASK_2) == QOI_OP_LUMA)
                {
                    const uint8_t data = NumericCast<uint8_t>(file.get());
                    const uint8_t vg = NumericCast<uint8_t>((tag & 0x3Fu) - 32);
                    prevPixel.Red   += NumericCast<uint8_t>(vg - 8u + ((data >> 4u) & 0xFu));
                    prevPixel.Green += vg;
                    prevPixel.Blue  += NumericCast<uint8_t>(vg - 8u + ((data >> 0u) & 0xFu));
                }
                else if((tag & QOI_MASK_2) == QOI_OP_RUN)
                {
                    const uint8_t run = tag & 0x3Fu;
                    for(std::size_t j = 0; j < run; ++j)
                    {
                        m_data[pixelIndex++] = prevPixel.Red;
                        m_data[pixelIndex++] = prevPixel.Green;
                        m_data[pixelIndex++] = prevPixel.Blue;

                        if(m_bitsPerPixel / 8 == 4)
                            m_data[pixelIndex++] = prevPixel.Alpha;
                    }
                }
            }

            const uint32_t pixelHashIndex = (QOI_COLOR_HASH(prevPixel) % 64);
            prevPixels[NumericCast<std::size_t>(pixelHashIndex)] = prevPixel;
        }

        m_data[pixelIndex++] = prevPixel.Red;
        m_data[pixelIndex++] = prevPixel.Green;
        m_data[pixelIndex++] = prevPixel.Blue;

        if(m_bitsPerPixel / 8 == 4)
            m_data[pixelIndex++] = prevPixel.Alpha;
    }
}