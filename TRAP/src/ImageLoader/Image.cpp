#include "TRAPPCH.h"
#include "Image.h"

#include "VFS/VFS.h"
#include "Utils/String.h"

#include "DefaultImage.h"
#include "PortableMaps/PGMImage.h"
#include "PortableMaps/PPMImage.h"
#include "PortableMaps/PNMImage.h"
#include "PortableMaps/PAMImage.h"
#include "PortableMaps/PFMImage.h"
#include "TARGA/TGAImage.h"
#include "Maths/Maths.h"

std::unique_ptr<TRAP::Image> TRAP::Image::LoadFromFile(const std::string& filepath)
{
	std::string virtualFilePath = VFS::Get()->MakeVirtualPathCompatible(filepath);
	const std::string fileFormat = Utils::String::GetSuffix(Utils::String::ToLower(virtualFilePath));

	std::unique_ptr<Image> result;

	if (fileFormat == "pgm")
		result = std::make_unique<INTERNAL::PGMImage>(virtualFilePath);
	else if (fileFormat == "ppm")
		result = std::make_unique<INTERNAL::PPMImage>(virtualFilePath);
	else if (fileFormat == "pnm")
		result = std::make_unique<INTERNAL::PNMImage>(virtualFilePath);
	else if (fileFormat == "pam")
		result = std::make_unique<INTERNAL::PAMImage>(virtualFilePath);
	else if (fileFormat == "pfm")
		result = std::make_unique<INTERNAL::PFMImage>(virtualFilePath);
	else if (fileFormat == "tga" || fileFormat == "icb" || fileFormat == "vda" || fileFormat == "vst")
		result = std::make_unique<INTERNAL::TGAImage>(virtualFilePath);
	else
	{
		TP_ERROR("[Image] Unsupported or unknown Image Format!");
		TP_WARN("[Image] Using Default Image!");
		return std::make_unique<INTERNAL::DefaultImage>(virtualFilePath);
	}

	//Test for Errors
	if (result->GetPixelDataSize() == 0)
		result = std::make_unique<INTERNAL::DefaultImage>(virtualFilePath);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Image> TRAP::Image::LoadFallback()
{
	return std::make_unique<INTERNAL::DefaultImage>("");
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::FlipY(const unsigned int width, const unsigned int height, ImageFormat format, uint8_t* data)
{
	if (data)
	{
		unsigned int yt, yb;

		switch (format)
		{
		case ImageFormat::Gray_Scale:
		{
			std::vector<uint8_t> newData;
			newData.assign(data, data + width * height);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const uint8_t temp{ newData[(x + yb * width) + 0] };

					newData[x + yb * width + 0] = newData[x + yt * width + 0];

					newData[x + yt * width + 0] = temp;
				}

			return newData;
		}

		case ImageFormat::Gray_Scale_Alpha:
		{
			std::vector<uint8_t> newData;
			newData.assign(data, data + width * height * 2);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const TRAP::Maths::tVec2<uint8_t> temp{ newData[2 * (x + yb * width) + 0], newData[2 * (x + yb * width) + 1] };

					newData[2 * (x + yb * width) + 0] = newData[2 * (x + yt * width) + 0];
					newData[2 * (x + yb * width) + 1] = newData[2 * (x + yt * width) + 1];

					newData[2 * (x + yt * width) + 0] = temp.x;
					newData[2 * (x + yt * width) + 1] = temp.y;
				}

			return newData;
		}

		case ImageFormat::RGB:
		{
			std::vector<uint8_t> newData;
			newData.assign(data, data + width * height * 3);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const TRAP::Maths::tVec3<uint8_t> temp{ newData[3 * (x + yb * width) + 0], newData[3 * (x + yb * width) + 1], newData[3 * (x + yb * width) + 2] };

					newData[3 * (x + yb * width) + 0] = newData[3 * (x + yt * width) + 0];
					newData[3 * (x + yb * width) + 1] = newData[3 * (x + yt * width) + 1];
					newData[3 * (x + yb * width) + 2] = newData[3 * (x + yt * width) + 2];

					newData[3 * (x + yt * width) + 0] = temp.x;
					newData[3 * (x + yt * width) + 1] = temp.y;
					newData[3 * (x + yt * width) + 2] = temp.z;
				}

			return newData;
		}

		case ImageFormat::RGBA:
		{
			std::vector<uint8_t> newData;
			newData.assign(data, data + width * height * 4);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const Maths::tVec4<uint8_t> temp{ newData[4 * (x + yb * width) + 0], newData[4 * (x + yb * width) + 1], newData[4 * (x + yb * width) + 2], newData[4 * (x + yb * width) + 3] };

					newData[4 * (x + yb * width) + 0] = newData[4 * (x + yt * width) + 0];
					newData[4 * (x + yb * width) + 1] = newData[4 * (x + yt * width) + 1];
					newData[4 * (x + yb * width) + 2] = newData[4 * (x + yt * width) + 2];
					newData[4 * (x + yb * width) + 3] = newData[4 * (x + yt * width) + 3];

					newData[4 * (x + yt * width) + 0] = temp.x;
					newData[4 * (x + yt * width) + 1] = temp.y;
					newData[4 * (x + yt * width) + 2] = temp.z;
					newData[4 * (x + yt * width) + 3] = temp.w;
				}

			return newData;
		}

		default:
			return {};
		}
	}
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint16_t> TRAP::Image::FlipY(const unsigned int width, const unsigned int height, ImageFormat format, uint16_t* data)
{
	if (data)
	{
		unsigned int yt, yb;

		switch (format)
		{
		case ImageFormat::Gray_Scale:
		{
			std::vector<uint16_t> newData;
			newData.assign(data, data + width * height);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const uint16_t temp{ newData[(x + yb * width) + 0] };

					newData[x + yb * width + 0] = newData[x + yt * width + 0];

					newData[x + yt * width + 0] = temp;
				}

			return newData;
		}

		case ImageFormat::Gray_Scale_Alpha:
		{
			std::vector<uint16_t> newData;
			newData.assign(data, data + width * height * 2);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const TRAP::Maths::tVec2<uint16_t> temp{ newData[2 * (x + yb * width) + 0], newData[2 * (x + yb * width) + 1] };

					newData[2 * (x + yb * width) + 0] = newData[2 * (x + yt * width) + 0];
					newData[2 * (x + yb * width) + 1] = newData[2 * (x + yt * width) + 1];

					newData[2 * (x + yt * width) + 0] = temp.x;
					newData[2 * (x + yt * width) + 1] = temp.y;
				}

			return newData;
		}

		case ImageFormat::RGB:
		{
			std::vector<uint16_t> newData;
			newData.assign(data, data + width * height * 3);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const TRAP::Maths::tVec3<uint16_t> temp{ newData[3 * (x + yb * width) + 0], newData[3 * (x + yb * width) + 1], newData[3 * (x + yb * width) + 2] };

					newData[3 * (x + yb * width) + 0] = newData[3 * (x + yt * width) + 0];
					newData[3 * (x + yb * width) + 1] = newData[3 * (x + yt * width) + 1];
					newData[3 * (x + yb * width) + 2] = newData[3 * (x + yt * width) + 2];

					newData[3 * (x + yt * width) + 0] = temp.x;
					newData[3 * (x + yt * width) + 1] = temp.y;
					newData[3 * (x + yt * width) + 2] = temp.z;
				}

			return newData;
		}

		case ImageFormat::RGBA:
		{
			std::vector<uint16_t> newData;
			newData.assign(data, data + width * height * 4);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const Maths::tVec4<uint16_t> temp{ newData[4 * (x + yb * width) + 0], newData[4 * (x + yb * width) + 1], newData[4 * (x + yb * width) + 2], newData[4 * (x + yb * width) + 3] };

					newData[4 * (x + yb * width) + 0] = newData[4 * (x + yt * width) + 0];
					newData[4 * (x + yb * width) + 1] = newData[4 * (x + yt * width) + 1];
					newData[4 * (x + yb * width) + 2] = newData[4 * (x + yt * width) + 2];
					newData[4 * (x + yb * width) + 3] = newData[4 * (x + yt * width) + 3];

					newData[4 * (x + yt * width) + 0] = temp.x;
					newData[4 * (x + yt * width) + 1] = temp.y;
					newData[4 * (x + yt * width) + 2] = temp.z;
					newData[4 * (x + yt * width) + 3] = temp.w;
				}

			return newData;
		}

		default:
			return {};
		}
	}
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<float> TRAP::Image::FlipY(const unsigned int width, const unsigned int height, ImageFormat format, float* data)
{
	if (data)
	{
		unsigned int yt, yb;

		switch (format)
		{
		case ImageFormat::Gray_Scale:
		{
			std::vector<float> newData;
			newData.assign(data, data + width * height);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const float temp{ newData[(x + yb * width) + 0] };

					newData[x + yb * width + 0] = newData[x + yt * width + 0];

					newData[x + yt * width + 0] = temp;
				}

			return newData;
		}

		case ImageFormat::Gray_Scale_Alpha:
		{
			std::vector<float> newData;
			newData.assign(data, data + width * height * 2);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const TRAP::Maths::Vec2 temp{ newData[2 * (x + yb * width) + 0], newData[2 * (x + yb * width) + 1] };

					newData[2 * (x + yb * width) + 0] = newData[2 * (x + yt * width) + 0];
					newData[2 * (x + yb * width) + 1] = newData[2 * (x + yt * width) + 1];

					newData[2 * (x + yt * width) + 0] = temp.x;
					newData[2 * (x + yt * width) + 1] = temp.y;
				}

			return newData;
		}

		case ImageFormat::RGB:
		{
			std::vector<float> newData;
			newData.assign(data, data + width * height * 3);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const TRAP::Maths::Vec3 temp{ newData[3 * (x + yb * width) + 0], newData[3 * (x + yb * width) + 1], newData[3 * (x + yb * width) + 2] };

					newData[3 * (x + yb * width) + 0] = newData[3 * (x + yt * width) + 0];
					newData[3 * (x + yb * width) + 1] = newData[3 * (x + yt * width) + 1];
					newData[3 * (x + yb * width) + 2] = newData[3 * (x + yt * width) + 2];

					newData[3 * (x + yt * width) + 0] = temp.x;
					newData[3 * (x + yt * width) + 1] = temp.y;
					newData[3 * (x + yt * width) + 2] = temp.z;
				}

			return newData;
		}

		case ImageFormat::RGBA:
		{
			std::vector<float> newData;
			newData.assign(data, data + width * height * 4);

			for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
				for (unsigned int x = 0; x < width; x++)
				{
					const Maths::Vec4 temp{ newData[4 * (x + yb * width) + 0], newData[4 * (x + yb * width) + 1], newData[4 * (x + yb * width) + 2], newData[4 * (x + yb * width) + 3] };

					newData[4 * (x + yb * width) + 0] = newData[4 * (x + yt * width) + 0];
					newData[4 * (x + yb * width) + 1] = newData[4 * (x + yt * width) + 1];
					newData[4 * (x + yb * width) + 2] = newData[4 * (x + yt * width) + 2];
					newData[4 * (x + yb * width) + 3] = newData[4 * (x + yt * width) + 3];

					newData[4 * (x + yt * width) + 0] = temp.x;
					newData[4 * (x + yt * width) + 1] = temp.y;
					newData[4 * (x + yt * width) + 2] = temp.z;
					newData[4 * (x + yt * width) + 3] = temp.w;
				}

			return newData;
		}

		default:
			return {};
		}
	}
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Image::FlipX(const unsigned int width, const unsigned int height, ImageFormat format, uint8_t* data)
{
	if (data)
	{
		unsigned int xt, xb;

		switch (format)
		{
		case ImageFormat::Gray_Scale:
		{
			std::vector<uint8_t> newData;
			newData.assign(data, data + width * height);

			for (xt = 0, xb = width - 1; xt <= xb; xt++, xb--)
				for (unsigned int y = 0; y < height; y++)
				{
					const uint8_t temp{ newData[(y + xb * height) + 0] };

					newData[y + xb * height + 0] = newData[y + xt * height + 0];

					newData[y + xt * height + 0] = temp;
				}

			return newData;
		}

		case ImageFormat::Gray_Scale_Alpha:
		{
			std::vector<uint8_t> newData;
			newData.assign(data, data + width * height * 2);

			for (xt = 0, xb = width - 1; xt <= xb; xt++, xb--)
				for (unsigned int y = 0; y < height; y++)
				{
					const TRAP::Maths::tVec2<uint8_t> temp{ newData[2 * (y + xb * height) + 0], newData[2 * (y + xb * height) + 1] };

					newData[2 * (y + xb * height) + 0] = newData[2 * (y + xt * height) + 0];
					newData[2 * (y + xb * height) + 1] = newData[2 * (y + xt * height) + 1];

					newData[2 * (y + xt * height) + 0] = temp.x;
					newData[2 * (y + xt * height) + 1] = temp.y;
				}

			return newData;
		}

		case ImageFormat::RGB:
		{
			std::vector<uint8_t> newData;
			newData.assign(data, data + width * height * 3);

			for (xt = 0, xb = width - 1; xt <= xb; xt++, xb--)
				for (unsigned int y = 0; y < height; y++)
				{
					const TRAP::Maths::tVec3<uint8_t> temp{ newData[3 * (y + xb * height) + 0], newData[3 * (y + xb * height) + 1], newData[3 * (y + xb * height) + 2] };

					newData[3 * (y + xb * height) + 0] = newData[3 * (y + xt * height) + 0];
					newData[3 * (y + xb * height) + 1] = newData[3 * (y + xt * height) + 1];
					newData[3 * (y + xb * height) + 2] = newData[3 * (y + xt * height) + 2];

					newData[3 * (y + xt * height) + 0] = temp.x;
					newData[3 * (y + xt * height) + 1] = temp.y;
					newData[3 * (y + xt * height) + 2] = temp.z;
				}

			return newData;
		}

		case ImageFormat::RGBA:
		{
			std::vector<uint8_t> newData;
			newData.assign(data, data + width * height * 4);

			for (xt = 0, xb = width - 1; xt <= xb; xt++, xb--)
				for (unsigned int y = 0; y < height; y++)
				{
					const Maths::tVec4<uint8_t> temp{ newData[4 * (y + xb * height) + 0], newData[4 * (y + xb * height) + 1], newData[4 * (y + xb * height) + 2], newData[4 * (y + xb * height) + 3] };

					newData[4 * (y + xb * height) + 0] = newData[4 * (y + xt * height) + 0];
					newData[4 * (y + xb * height) + 1] = newData[4 * (y + xt * height) + 1];
					newData[4 * (y + xb * height) + 2] = newData[4 * (y + xt * height) + 2];
					newData[4 * (y + xb * height) + 3] = newData[4 * (y + xt * height) + 3];

					newData[4 * (y + xt * height) + 0] = temp.x;
					newData[4 * (y + xt * height) + 1] = temp.y;
					newData[4 * (y + xt * height) + 2] = temp.z;
					newData[4 * (y + xt * height) + 3] = temp.w;
				}

			return newData;
		}

		default:
			return {};
		}
	}
	return {};
}