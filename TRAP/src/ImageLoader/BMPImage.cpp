#include "TRAPPCH.h"
#include "BMPImage.h"

TRAP::BMPImage::BMPImage(const std::string& path)
	: m_bitsPerPixel(0), m_width(0), m_height(0), m_size(0)
{
	std::filesystem::path physicalPath;
	if (!VFS::Get()->ResolveReadPhysicalPath(path, physicalPath))
	{
		TP_ERROR("[BMPImage] Couldn't resolve FilePath: ", path, "!");
		TP_WARN("[BMPImage] Using Default Texture");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR("[BMPImage] Couldn't open FilePath: ", path, "!");
			TP_WARN("[BMPImage] Using Default Texture");
			return;
		}

		file.seekg(0, std::ios::end);
		const std::size_t length = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<uint8_t> fileInfo(length);
		file.read(reinterpret_cast<char*>(fileInfo.data()), 54);

		if(fileInfo[0] != 'B' && fileInfo[1] != 'M')
		{
			file.close();
			TP_ERROR("[BMPImage] Invalid File Format. Bitmap Required. FilePath: ", path, "!");
			TP_WARN("[BMPImage] Using Default Texture");
			return;
		}

		if(fileInfo[28] != 24 && fileInfo[28] != 32)
		{
			file.close();
			TP_ERROR("[BMPImage] Invalid File Format. 24 or 32 Bit Image Required. FilePath: ", path, "!");
			TP_WARN("[BMPImage] Using Default Texture");
			return;
		}

		m_bitsPerPixel = fileInfo[28];
		m_width = fileInfo[18] + (fileInfo[19] << 8);
		m_height = fileInfo[22] + (fileInfo[23] << 8);
		const uint32_t pixelsOffset = fileInfo[10] + (fileInfo[11] << 8);
		m_size = (m_width * m_bitsPerPixel + 31) / 32 * 4 * m_height;
		m_data.resize(m_size);

		file.seekg(pixelsOffset, std::ios::beg);
		file.read(reinterpret_cast<char*>(m_data.data()), m_size);
		file.close();
	}
}