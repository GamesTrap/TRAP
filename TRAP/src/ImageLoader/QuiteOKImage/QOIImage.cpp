#include "TRAPPCH.h"
#include "QOIImage.h"

#include "Utils/String/String.h"
#include "FS/FS.h"
#include "Utils/ByteSwap.h"
#include "Utils/Utils.h"
#include "ImageLoader/Image.h"

static constexpr std::array<uint8_t, 8> EndMarker{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::QOIImage::QOIImage(std::filesystem::path filepath)
{
	TP_PROFILE_FUNCTION();

	m_filepath = std::move(filepath);

	TP_DEBUG(Log::ImageQOIPrefix, "Loading image: \"", m_filepath.generic_u8string(), "\"");

	if (!FS::FileOrFolderExists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImageQOIPrefix, "Couldn't open file path: ", m_filepath.generic_u8string(), "!");
		TP_WARN(Log::ImageQOIPrefix, "Using default image!");
		return;
	}

    file.seekg(0, std::ios::end);
    const std::size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if(fileSize < sizeof(Header) + EndMarker.size())
    {
        TP_ERROR(Log::ImageQOIPrefix, "File size is too small: ", m_filepath.generic_u8string(), "!");
        TP_WARN(Log::ImageQOIPrefix, "Using default image!");
        return;
    }

    Header header{};
    file.read(header.MagicNumber.data(), 4);
    file.read(reinterpret_cast<char*>(&header.Width), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&header.Height), sizeof(uint32_t));
    file >> header.Channels >> header.ColorSpace;

    //Magic number must be "qoif"
    if(strcmp(header.MagicNumber.data(), "qoif") != 0)
    {
        TP_ERROR(Log::ImageQOIPrefix, "Invalid magic number: ", header.MagicNumber.data(), "!");
        TP_WARN(Log::ImageQOIPrefix, "Using default image!");
        return;
    }

	//Height and width uses big-endian
	bool needSwap = Utils::GetEndian() != Utils::Endian::Big;

    //Convert to machines endian
    if(needSwap)
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

    m_data.resize(header.Width * header.Height * header.Channels);
    m_bitsPerPixel = header.Channels * 8;
    m_colorFormat = header.Channels == 3 ? ColorFormat::RGB : ColorFormat::RGBA;

    DecodeImage(file, fileSize);

    file.close();
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::INTERNAL::QOIImage::GetPixelData() const
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::INTERNAL::QOIImage::GetPixelDataSize() const
{
	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

struct Pixel
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t Alpha;
};

static constexpr uint8_t QOI_RGB = 0xFE;
static constexpr uint8_t QOI_RGBA = 0xFF;
static constexpr uint8_t QOI_OP_INDEX = 0x00;
static constexpr uint8_t QOI_OP_DIFF = 0x40;
static constexpr uint8_t QOI_OP_LUMA = 0x80;
static constexpr uint8_t QOI_OP_RUN = 0xC0;
static constexpr uint8_t QOI_MASK_2 = 0xC0;

static constexpr int32_t QOI_COLOR_HASH(const Pixel& p)
{
    return p.Red * 3 + p.Green * 5 + p.Blue * 7 + p.Alpha * 11;
}

void TRAP::INTERNAL::QOIImage::DecodeImage(std::ifstream& file, const std::size_t& fileSize)
{
    Pixel px{0, 0, 0, 255};
    std::array<Pixel, 64> index{};

    int32_t totalChunksLength = fileSize - EndMarker.size();
    int32_t pxLen = m_width * m_height * m_bitsPerPixel / 8;
    int32_t run = 0;
    int32_t p = 0;

    for(int32_t pxPos = 0; pxPos < pxLen; pxPos += m_bitsPerPixel / 8)
    {
        if(run > 0)
            --run;
        else if(p < totalChunksLength)
        {
            int8_t b1 = 0;
            file.read(reinterpret_cast<char*>(&b1), 1);

            if(b1 == QOI_RGB)
            {
                file.read(reinterpret_cast<char*>(&px.Red), 1);
                file.read(reinterpret_cast<char*>(&px.Green), 1);
                file.read(reinterpret_cast<char*>(&px.Blue), 1);
            }
            else if(b1 == QOI_RGBA)
            {
                file.read(reinterpret_cast<char*>(&px.Red), 1);
                file.read(reinterpret_cast<char*>(&px.Green), 1);
                file.read(reinterpret_cast<char*>(&px.Blue), 1);
                file.read(reinterpret_cast<char*>(&px.Alpha), 1);
            }
            else if((b1 & QOI_MASK_2) == QOI_OP_INDEX)
                px = index[b1];
            else if((b1 & QOI_MASK_2) == QOI_OP_DIFF)
            {
				px.Red   += ((static_cast<int32_t>(b1) >> 4) & 0x03) - 2;
				px.Green += ((static_cast<int32_t>(b1) >> 2) & 0x03) - 2;
				px.Blue  += ( static_cast<int32_t>(b1)       & 0x03) - 2;
            }
            else if((b1 & QOI_MASK_2) == QOI_OP_LUMA)
            {
				int8_t b2 = 0;
                file.read(reinterpret_cast<char*>(&b2), 1);
				int32_t vg = (static_cast<int32_t>(b1) & 0x3f) - 32;
				px.Red   += vg - 8 + ((static_cast<int32_t>(b2) >> 4) & 0x0f);
				px.Green += vg;
				px.Blue  += vg - 8 +  (static_cast<int32_t>(b2)       & 0x0f);
            }
            else if((b1 & QOI_MASK_2) == QOI_OP_RUN)
                run = (static_cast<int32_t>(b1) & 0x3F);

            index[QOI_COLOR_HASH(px) % 64] = px;
        }

        m_data[pxPos + 0] = px.Red;
        m_data[pxPos + 1] = px.Green;
        m_data[pxPos + 2] = px.Blue;

        if(m_bitsPerPixel / 8 == 4)
            m_data[pxPos + 3] = px.Alpha;
    }
}