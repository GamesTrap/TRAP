#ifndef _TRAP_TEXTURE_H_
#define _TRAP_TEXTURE_H_

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

	using TextureUsage = BufferUsage;

	class Texture
	{
	protected:
		Texture();

	public:
		~Texture() = default;
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
		TextureUsage GetTextureUsage() const;
		Image::ColorFormat GetColorFormat() const;
		uint32_t GetBitsPerChannel() const;
		uint32_t GetBytesPerChannel() const;
		uint32_t GetBitsPerPixel() const;
		uint32_t GetBytesPerPixel() const;

		TRAP::Ref<TRAP::Graphics::TextureBase> GetTexture() const;

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

		std::string m_name;
		TextureType m_textureType;
		TextureUsage m_textureUsage;
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

	for(uint32_t i = 0; i < images.size(); ++i)
		images[i] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[i]);

	return images;
}

#endif /*_TRAP_TEXTURE_H_*/