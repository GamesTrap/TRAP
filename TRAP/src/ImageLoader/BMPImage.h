#ifndef _TRAP_BMPIMAGE_H_
#define _TRAP_BMPIMAGE_H_

namespace TRAP
{
	class BMPImage
	{
	public:
		explicit BMPImage(const std::string& path);
		~BMPImage() = default;
		BMPImage(const BMPImage&) = default;
		BMPImage& operator=(const BMPImage&) = default;
		BMPImage(BMPImage&&) = default;
		BMPImage& operator=(BMPImage&&) = default;

		std::vector<uint8_t> GetData() const;
		uint32_t GetBitsPerPixel() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetSize() const;
		bool HasAlphaChannel() const;
		bool IsImageCompressed() const;

	private:
		union PixelInfo
		{
			uint32_t Color;
			struct
			{
				uint8_t B, G, R, A;
			};
		};

		std::vector<uint8_t> m_data;
		uint32_t m_bitsPerPixel;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_size;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::vector<uint8_t> TRAP::BMPImage::GetData() const
{
	return m_data;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::BMPImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::BMPImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::BMPImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::BMPImage::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::BMPImage::HasAlphaChannel() const
{
	return m_bitsPerPixel == 32;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::BMPImage::IsImageCompressed() const
{
	return false;
}

#endif /*_TRAP_BMPIMAGE_H_*/