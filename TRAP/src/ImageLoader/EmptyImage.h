#ifndef _TRAP_EMPTYIMAGE_H_
#define _TRAP_EMPTYIMAGE_H_

#include "Image.h"

namespace TRAP::INTERNAL
{
	class EmptyImage final : public Image
	{
	public:
		explicit EmptyImage(ImageFormat format, uint32_t width, uint32_t height);

		void* GetPixelData() override;
		uint32_t GetPixelDataSize() const override;
		uint32_t GetBitsPerPixel() const override;
		uint32_t GetBytesPerPixel() const override;
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		bool HasAlphaChannel() const override;
		bool IsImageCompressed() const override;
		bool IsImageGrayScale() const override;
		bool IsImageColored() const override;
		bool IsHDR() const override;
		std::string GetFilePath() const override;
		ImageFormat GetFormat() const override;

	private:
		ImageFormat m_format;
		bool m_isImageColored;
		bool m_isImageGrayScale;
		bool m_hasAlphaChannel;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_bitPerPixel;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline void* TRAP::INTERNAL::EmptyImage::GetPixelData()
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::INTERNAL::EmptyImage::GetPixelDataSize() const
{
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::INTERNAL::EmptyImage::GetBitsPerPixel() const
{
	return m_bitPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::INTERNAL::EmptyImage::GetBytesPerPixel() const
{
	return m_bitPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::INTERNAL::EmptyImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::INTERNAL::EmptyImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::INTERNAL::EmptyImage::HasAlphaChannel() const
{
	return m_hasAlphaChannel;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::INTERNAL::EmptyImage::IsImageCompressed() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::INTERNAL::EmptyImage::IsImageGrayScale() const
{
	return m_isImageGrayScale;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::INTERNAL::EmptyImage::IsImageColored() const
{
	return m_isImageColored;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::INTERNAL::EmptyImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::INTERNAL::EmptyImage::GetFilePath() const
{
	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::ImageFormat TRAP::INTERNAL::EmptyImage::GetFormat() const
{
	return m_format;
}

#endif /*_TRAP_EMPTYIMAGE_H_*/