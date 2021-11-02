#ifndef TRAP_TEXTURE_H
#define TRAP_TEXTURE_H

#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/API/ResourceLoader.h"

namespace TRAP
{
	class Image;
}

namespace TRAP::Graphics
{
	class TextureBase;

	enum class TextureType
	{
		Texture2D,
		TextureCube
	};

	class Texture
	{
	protected:
		Texture();

	public:
		virtual ~Texture() = default;
		Texture(const Texture&) = default;
		Texture& operator=(const Texture&) = default;
		Texture(Texture&&) = default;
		Texture& operator=(Texture&&) = default;

		const std::string& GetName() const;
		TextureType GetType() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		virtual uint32_t GetDepth() const = 0;
		virtual uint32_t GetArraySize() const = 0;
		uint32_t GetMipLevels() const;
		Image::ColorFormat GetColorFormat() const;
		uint32_t GetBitsPerChannel() const;
		uint32_t GetBytesPerChannel() const;
		uint32_t GetBitsPerPixel() const;
		uint32_t GetBytesPerPixel() const;
		uint32_t GetMipWidth(uint32_t mipLevel) const;
		uint32_t GetMipHeight(uint32_t mipLevel) const;
		Math::Vec2ui GetMipSize(uint32_t mipLevel) const;

		TRAP::Ref<TRAP::Graphics::TextureBase> GetTexture() const;

		//Data array length and sizeInBytes must match the textures current size or it won't update
		virtual void Update(const void* data, uint32_t sizeInBytes, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) = 0;

		bool IsLoaded() const;
		void AwaitLoading() const;

		static uint32_t CalculateMipLevels(uint32_t width, uint32_t height);
		template<typename T>
		static std::array<TRAP::Scope<TRAP::Image>, 6> SplitImageFromCross(const TRAP::Scope<TRAP::Image>& image);
	protected:
		static API::ImageFormat ColorFormatBitsPerPixelToImageFormat(Image::ColorFormat colorFormat, uint32_t bpp);
		static Image::ColorFormat ImageFormatToColorFormat(API::ImageFormat imageFormat);
		static uint32_t GetBitsPerChannelFromImageFormat(API::ImageFormat imageFormat);
		static TRAP::Scope<TRAP::Image> Rotate90Clockwise(const TRAP::Scope<TRAP::Image>& img);
		static TRAP::Scope<TRAP::Image> Rotate90CounterClockwise(const TRAP::Scope<TRAP::Image>& img);

		std::string m_name;
		TextureType m_textureType;
		API::SyncToken m_syncToken;

		TRAP::Ref<TRAP::Graphics::TextureBase> m_texture;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
std::array<TRAP::Scope<TRAP::Image>, 6> TRAP::Graphics::Texture::SplitImageFromCross(const TRAP::Scope<TRAP::Image>& image)
{
	const bool isHorizontal = image->GetWidth() > image->GetHeight();

	const uint32_t stride = image->GetBytesPerPixel();
	uint32_t face = 0;
	uint32_t cxLimit = 4, cyLimit = 3;
	if(!isHorizontal)
	{
		cxLimit = 3;
		cyLimit = 4;
	}
	uint32_t faceWidth = image->GetWidth() / cxLimit;
	uint32_t faceHeight = image->GetHeight() / cyLimit;

	std::array<std::vector<T>, 6> cubeTextureData;
	for(auto& i : cubeTextureData)
		i.resize(faceWidth * faceHeight * stride);

	for(uint32_t cy = 0; cy < cyLimit; ++cy)
	{
		for(uint32_t cx = 0; cx < cxLimit; ++cx)
		{
			if((cy == 0 || cy == 2 || cy == 3) && cx != 1)
				continue;

			for(uint32_t y = 0; y < faceHeight; ++y)
			{
				uint32_t offset = y;
				if(!isHorizontal && face == 5)
					offset = faceHeight - (y + 1);
				const uint32_t yp = cy * faceHeight + offset;

				for(uint32_t x = 0; x < faceWidth; ++x)
				{
					offset = x;
					if(!isHorizontal && face == 5)
						offset = faceWidth - (x + 1);
					const uint32_t xp = cx * faceWidth + offset;
					switch(stride)
					{
					case 1:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
						break;

					case 2:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 1];
						break;

					case 3:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 1];
						cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 2];
						break;

					case 4:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 1];
						cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 2];
						cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 3];
						break;

					default:
						break;
					}
				}
			}
			++face;
		}
	}

	std::array<TRAP::Scope<TRAP::Image>, 6> images{};

	//Load Images in correct order
	if(cxLimit == 4 && cyLimit == 3)
	{
		images[0] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[3]); //+X
		images[1] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[1]); //-X
		images[2] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[0]); //+Y
		images[3] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[5]); //-Y
		images[4] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[2]); //+Z
		images[5] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[4]); //-Z
	}
	else
	{
		images[0] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[2]); //+X
		images[1] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[5]); //-X
		images[2] = Rotate90CounterClockwise(TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[0])); //+Y
		images[3] = Rotate90Clockwise(TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[4])); //-Y
		images[4] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[1]); //+Z
		images[5] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[3]); //-Z
	}

	return images;
}


#endif /*TRAP_TEXTURE_H*/