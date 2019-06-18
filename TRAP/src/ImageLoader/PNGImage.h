#ifndef _TRAP_PNGIMAGE_H_
#define _TRAP_PNGIMAGE_H_

namespace TRAP
{
	class PNGImage
	{
	public:
		explicit PNGImage(const std::string& path);
		~PNGImage() = default;
		PNGImage(const PNGImage&) = default;
		PNGImage& operator=(const PNGImage&) = default;
		PNGImage(PNGImage&&) = default;
		PNGImage& operator=(PNGImage&&) = default;

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

inline std::vector<uint8_t> TRAP::PNGImage::GetData() const
{
	return m_data;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::PNGImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::PNGImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::PNGImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::PNGImage::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::PNGImage::HasAlphaChannel() const
{
	return m_bitsPerPixel == 32;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::PNGImage::IsImageCompressed() const
{
	return m_compressed;
}

#endif /*_TRAP_PNGIMAGE_H_*/