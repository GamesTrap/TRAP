#include "TRAPPCH.h"
#include "TGAImage.h"

TRAP::TGAImage::TGAImage(const std::string& path)
	: m_bitsPerPixel(0), m_width(0), m_height(0), m_size(0), m_compressed(false)
{
	std::filesystem::path physicalPath;
	if(!VFS::Get()->ResolveReadPhysicalPath(path, physicalPath))
	{
		TP_ERROR("[TGAImage] Couldn't resolve FilePath: ", path, "!");
		TP_WARN("[TGAImage] Using Default Texture");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR("[TGAImage] Couldn't open FilePath: ", path, "!");
			TP_WARN("[TGAImage] Using Default Texture");
			return;
		}

		std::array<uint8_t, 18> header = {0};
		static std::uint8_t DeCompressed[12] = { 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
		static std::uint8_t IsCompressed[12] = { 0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

		file.read(reinterpret_cast<char*>(header.data()), sizeof(header));

		if(!std::memcmp(DeCompressed, header.data(), sizeof(DeCompressed)))
		{
			m_bitsPerPixel = header[16];
			m_width = header[13] * 256 + header[12];
			m_height = header[15] * 256 + header[14];
			m_size = (m_width * m_bitsPerPixel + 31) / 32 * 4 * m_height;

			if(m_bitsPerPixel != 24 && m_bitsPerPixel != 32)
			{
				file.close();
				TP_ERROR("[TGAImage] Invalid Image Format. Required: 24 or 32 Bit Image. FilePath: ", path, "!");
				TP_WARN("[TGAImage] Using Default Texture");
				return;
			}

			m_data.resize(m_size);
			m_compressed = false;
			file.read(reinterpret_cast<char*>(m_data.data()), m_size);
		}
		else if(!std::memcmp(IsCompressed, header.data(), sizeof(IsCompressed)))
		{
			m_bitsPerPixel = header[16];
			m_width = header[13] * 256 + header[12];
			m_height = header[15] * 256 + header[14];
			m_size = (m_width * m_bitsPerPixel + 31) / 32 * 4 * m_height;

			if(m_bitsPerPixel != 24 && m_bitsPerPixel != 32)
			{
				file.close();
				TP_ERROR("[TGAImage] Invalid Image Format. Required: 24 or 32 Bit Image. FilePath: ", path, "!");
				TP_WARN("[TGAImage] Using Default Texture");
				return;
			}

			PixelInfo pixel = { 0 };
			int currentByte = 0;
			std::size_t currentPixel = 0;
			m_compressed = true;
			uint8_t chunkHeader = 0;
			const int bytesPerPixel = static_cast<int>(m_bitsPerPixel) / 8;
			m_data.resize(m_width * m_height * bytesPerPixel);

			do
			{
				file.read(reinterpret_cast<char*>(&chunkHeader), sizeof(chunkHeader));

				if(chunkHeader < 128)
				{
					++chunkHeader;
					for(int i = 0; i < chunkHeader; ++i, ++currentPixel)
					{
						file.read(reinterpret_cast<char*>(&pixel), bytesPerPixel);

						m_data[currentByte++] = pixel.B;
						m_data[currentByte++] = pixel.G;
						m_data[currentByte++] = pixel.R;							
						if (m_bitsPerPixel > 24)
							m_data[currentByte++] = pixel.A;
					}
				}
				else
				{
					chunkHeader -= 127;
					file.read(reinterpret_cast<char*>(&pixel), bytesPerPixel);

					for(int i = 0; i < chunkHeader; ++i, ++currentPixel)
					{
						m_data[currentByte++] = pixel.B;
						m_data[currentByte++] = pixel.G;
						m_data[currentByte++] = pixel.R;							
						if (m_bitsPerPixel > 24)
							m_data[currentByte++] = pixel.A;
					}
				}
			} while (currentPixel < m_width * m_height);
		}
		else
		{
			file.close();
			TP_ERROR("[TGAImage] Invalid Image Format. Required: 24 or 32 Bit Image. FilePath: ", path, "!");
			TP_WARN("[TGAImage] Using Default Texture");
			return;
		}

		file.close();
	}
}