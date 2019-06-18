#ifndef _TRAP_IMAGE_H_
#define _TRAP_IMAGE_H_

namespace TRAP
{
	class Image
	{
	public:
		Image();
		explicit Image(const std::string& path);
		~Image() = default;
		Image(const Image&) = default;
		Image& operator=(const Image&) = default;
		Image(Image&&) = default;
		Image& operator=(Image&&) = default;

		std::vector<uint8_t>& GetData();
		uint32_t GetBitsPerPixel() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetSize() const;
		bool HasAlphaChannel() const;
		bool IsImageCompressed() const;
		bool IsImageGrayScale() const;
		std::string GetFilePath() const;

		//Save Images in the future?

	private:
		void ConvertBGRToRGB();

		std::vector<uint8_t> m_data;
		uint32_t m_bitsPerPixel;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_size;
		bool m_compressed;
		std::string m_filePath;
	};
}

inline std::vector<uint8_t>& TRAP::Image::GetData()
{
	return m_data;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Image::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Image::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Image::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Image::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::Image::HasAlphaChannel() const
{
	return m_bitsPerPixel == 32;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::Image::IsImageCompressed() const
{
	return m_compressed;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::Image::IsImageGrayScale() const
{
	return m_bitsPerPixel == 8;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::Image::GetFilePath() const
{
	return m_filePath;
}

#endif /*_TRAP_IMAGE_H_*/