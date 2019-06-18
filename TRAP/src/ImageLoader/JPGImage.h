#ifndef _TRAP_JPGIMAGE_H_
#define _TRAP_JPGIMAGE_H_

namespace TRAP
{
	class JPGImage
	{
	public:
		explicit JPGImage(const std::string& path);
		~JPGImage() = default;
		JPGImage(const JPGImage&) = default;
		JPGImage& operator=(const JPGImage&) = default;
		JPGImage(JPGImage&&) = default;
		JPGImage& operator=(JPGImage&&) = default;

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
		bool m_compressed;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::vector<uint8_t> TRAP::JPGImage::GetData() const
{
	return m_data;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::JPGImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::JPGImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::JPGImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::JPGImage::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::JPGImage::HasAlphaChannel() const
{
	return m_bitsPerPixel == 32;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::JPGImage::IsImageCompressed() const
{
	return m_compressed;
}

#endif /*_TRAP_JPGIMAGE_H_*/