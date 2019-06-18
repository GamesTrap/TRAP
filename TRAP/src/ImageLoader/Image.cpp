#include "TRAPPCH.h"
#include "Image.h"

TRAP::Image::Image()
	: m_bitsPerPixel(32), m_width(32), m_height(32), m_size(4096), m_compressed(true)
{	
	//Use Default Image Data
	m_data.insert(m_data.end(), ImageLoader::defaultImageData.data(), &ImageLoader::defaultImageData[ImageLoader::defaultImageData.size() - 1]);
}

TRAP::Image::Image(const std::string& path)
	: m_bitsPerPixel(0), m_width(0), m_height(0), m_size(0), m_compressed(false), m_filePath(VFS::Get()->MakeVirtualPathCompatible(path))
{
	std::vector<std::string> splittedPath = Utils::String::SplitString(path, '.');
	const std::string fileFormat = Utils::String::ToLower(splittedPath.back());
	if(fileFormat == "tga" || fileFormat == "icb" || fileFormat == "vda" || fileFormat == "vst")
	{
		TP_DEBUG("[Image][TGA] Loading Image: ", path);
		const TGAImage tgaImage(path);
		m_data = tgaImage.GetData();
		m_bitsPerPixel = tgaImage.GetBitsPerPixel();
		m_width = tgaImage.GetWidth();
		m_height = tgaImage.GetHeight();
		m_size = tgaImage.GetSize();
		m_compressed = tgaImage.IsImageCompressed();

		//Change BGR(A) to RGB(A)
		if (m_bitsPerPixel > 16)
			ConvertBGRToRGB();

		if (m_data.empty() || m_width == 0 || m_height == 0 || m_bitsPerPixel == 0 || m_size == 0)
		{
			//Use Default Image Data
			m_data.insert(m_data.end(), ImageLoader::defaultImageData.data(), &ImageLoader::defaultImageData[ImageLoader::defaultImageData.size() - 1]);
			m_bitsPerPixel = 32;
			m_width = 32;
			m_height = 32;
			m_size = 4096;
			m_compressed = true;
		}
	}
	else if(fileFormat == "bmp" || fileFormat == "rle" || fileFormat == "dib")
	{
		TP_DEBUG("[Image][BMP] Loading Image: ", path);
		const BMPImage bmpImage(path);
		m_data = bmpImage.GetData();
		m_bitsPerPixel = bmpImage.GetBitsPerPixel();
		m_width = bmpImage.GetWidth();
		m_height = bmpImage.GetHeight();
		m_size = bmpImage.GetSize();
		m_compressed = bmpImage.IsImageCompressed();

		//Change BGR(A) to RGB(A)
		if (m_bitsPerPixel > 16)
			ConvertBGRToRGB();

		if (m_data.empty() || m_width == 0 || m_height == 0 || m_bitsPerPixel == 0 || m_size == 0)
		{
			//Use Default Image Data
			m_data.insert(m_data.end(), ImageLoader::defaultImageData.data(), &ImageLoader::defaultImageData[ImageLoader::defaultImageData.size() - 1]);
			m_bitsPerPixel = 32;
			m_width = 32;
			m_height = 32;
			m_size = 4096;
			m_compressed = true;
		}
	}
	else if(fileFormat == "png")
	{
		//TODO PNG Here
		TP_WARN("[Image][PNG] WIP");
		/*TP_DEBUG("[Image][PNG] Loading Image: ", path);
		const PNGImage pngImage(path);
		m_data = pngImage.GetData();
		m_bitsPerPixel = pngImage.GetBitsPerPixel();
		m_width = pngImage.GetWidth();
		m_height = pngImage.GetHeight();
		m_size = pngImage.GetSize();
		m_compressed = pngImage.IsImageCompressed();*/

		if (m_data.empty() || m_width == 0 || m_height == 0 || m_bitsPerPixel == 0 || m_size == 0)
		{
			//Use Default Image Data
			m_data.insert(m_data.end(), ImageLoader::defaultImageData.data(), &ImageLoader::defaultImageData[ImageLoader::defaultImageData.size() - 1]);
			m_bitsPerPixel = 32;
			m_width = 32;
			m_height = 32;
			m_size = 4096;
			m_compressed = true;
		}
	}
	else if (fileFormat == "jpg" || fileFormat == "jpeg" || fileFormat == "jpe")
	{
		//TODO JPG Here
		TP_WARN("[Image][JPG] WIP");
		/*TP_DEBUG("[Image][PNG] Loading Image: ", path);
		const PNGImage pngImage(path);
		m_data = pngImage.GetData();
		m_bitsPerPixel = pngImage.GetBitsPerPixel();
		m_width = pngImage.GetWidth();
		m_height = pngImage.GetHeight();
		m_size = pngImage.GetSize();
		m_compressed = pngImage.IsImageCompressed();*/

		if (m_data.empty() || m_width == 0 || m_height == 0 || m_bitsPerPixel == 0 || m_size == 0)
		{
			//Use Default Image Data
			m_data.insert(m_data.end(), ImageLoader::defaultImageData.data(), &ImageLoader::defaultImageData[ImageLoader::defaultImageData.size() - 1]);
			m_bitsPerPixel = 32;
			m_width = 32;
			m_height = 32;
			m_size = 4096;
			m_compressed = true;
		}
	}
	else
	{	
		TP_ERROR("[Image] Couldn't open FilePath: ", path, "!");
		TP_WARN("[Image] Using Default Texture");

		//Use Default Image Data
		m_data.insert(m_data.end(), ImageLoader::defaultImageData.data(), &ImageLoader::defaultImageData[ImageLoader::defaultImageData.size() - 1]);
		m_bitsPerPixel = 32;
		m_width = 32;
		m_height = 32;
		m_size = 4096;
		m_compressed = true;
	}
}

void TRAP::Image::ConvertBGRToRGB()
{
	for(unsigned int i = 0; i < m_size; i += (m_bitsPerPixel / 8))
		//1st Byte XOR 3rd Byte XOR 1st Byte XOR 3rd Byte
		m_data[i] ^= m_data[i + 2] ^= m_data[i] ^= m_data[i + 2];
}