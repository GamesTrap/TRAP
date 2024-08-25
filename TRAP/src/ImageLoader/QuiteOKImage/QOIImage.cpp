#include "TRAPPCH.h"
#include "QOIImage.h"

#include "FileSystem/FileSystem.h"

#include <qoi.h>

TRAP::INTERNAL::QOIImage::QOIImage(std::filesystem::path filepath)
    : Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	TP_DEBUG(Log::ImageQOIPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

    qoi_desc qoiInfo{};
    u8* const pixelData = static_cast<u8*>(qoi_read(m_filepath.string().c_str(), &qoiInfo, 0));

    if(pixelData == nullptr)
    {
		TP_ERROR(Log::ImageQOIPrefix, "Failed to decode image: ", m_filepath, "!");
		TP_WARN(Log::ImageQOIPrefix, "Using default image!");
        return;
    }

    m_width = qoiInfo.width;
    m_height = qoiInfo.height;
    m_bitsPerPixel = 8u * qoiInfo.channels;
    m_colorFormat = qoiInfo.channels == 3u ? ColorFormat::RGB : ColorFormat::RGBA;

    const usize pixelDataSize = NumericCast<usize>(m_width) * m_height * qoiInfo.channels;
    m_data.assign(pixelData, pixelData + pixelDataSize);

    free(pixelData);
}
