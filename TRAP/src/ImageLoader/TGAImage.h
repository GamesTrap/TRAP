#ifndef _TRAP_TGAIMAGELOADER_H_
#define _TRAP_TGAIMAGELOADER_H_

namespace TRAP
{
	class TGAImage
	{
	public:
		explicit TGAImage(const std::string& path);
		~TGAImage() = default;
		TGAImage(const TGAImage&) = default;
		TGAImage& operator=(const TGAImage&) = default;
		TGAImage(TGAImage&&) = default;
		TGAImage& operator=(TGAImage&&) = default;

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
				uint8_t R, G, B, A;
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

inline std::vector<uint8_t> TRAP::TGAImage::GetData() const
{
	return m_data;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::TGAImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::TGAImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::TGAImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::TGAImage::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::TGAImage::HasAlphaChannel() const
{
	return m_bitsPerPixel == 32;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::TGAImage::IsImageCompressed() const
{
	return m_compressed;
}

#endif /*_TRAP_TGAIMAGELOADER_H_*/