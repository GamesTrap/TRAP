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

	/// <summary>
	/// Different texture types.
	/// </summary>
	enum class TextureType
	{
		Texture2D,
		TextureCube
	};

	class Texture
	{
	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		Texture();

	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Texture() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		Texture(const Texture&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Texture& operator=(const Texture&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Texture(Texture&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Texture& operator=(Texture&&) = default;

		/// <summary>
		/// Retrieve the name of the texture.
		/// </summary>
		/// <returns>Name of the texture.</returns>
		const std::string& GetName() const;
		/// <summary>
		/// Retrieve the texture type.
		/// </summary>
		/// <returns>Texture type.</returns>
		TextureType GetType() const;
		/// <summary>
		/// Retrieve the texture width.
		/// </summary>
		/// <returns>Texture width.</returns>
		uint32_t GetWidth() const;
		/// <summary>
		/// Retrieve the texture height.
		/// </summary>
		/// <returns>Texture height.</returns>
		uint32_t GetHeight() const;
		/// <summary>
		/// Retrieve the texture depth.
		/// </summary>
		/// <returns>Texture depth.</returns>
		virtual uint32_t GetDepth() const = 0;
		/// <summary>
		/// Retrieve the texture array size.
		/// </summary>
		/// <returns>Texture array size.</returns>
		virtual uint32_t GetArraySize() const = 0;
		/// <summary>
		/// Retrieve the textures mip level count.
		/// </summary>
		/// <returns>Textures mip level count.</returns>
		uint32_t GetMipLevels() const;
		/// <summary>
		/// Retrieve the textures color format.
		/// </summary>
		/// <returns>Textures color format.</returns>
		Image::ColorFormat GetColorFormat() const;
		/// <summary>
		/// Retrieve the textures bits per channel.
		/// </summary>
		/// <returns>Textures bits per channel.</returns>
		uint32_t GetBitsPerChannel() const;
		/// <summary>
		/// Retrieve the textures bytes per channel.
		/// </summary>
		/// <returns>Textures bytes per channel.</returns>
		uint32_t GetBytesPerChannel() const;
		/// <summary>
		/// Retrieve the textures bits per pixel.
		/// </summary>
		/// <returns>Textures bits per pixel.</returns>
		uint32_t GetBitsPerPixel() const;
		/// <summary>
		/// Retrieve the textures bytes per pixel.
		/// </summary>
		/// <returns>Textures bytes per pixel.</returns>
		uint32_t GetBytesPerPixel() const;
		/// <summary>
		/// Retrieve the textures mip width of a specific level.
		/// </summary>
		/// <param name="mipLevel">Mip level.</param>
		/// <returns>Mip width.</returns>
		uint32_t GetMipWidth(uint32_t mipLevel) const;
		/// <summary>
		/// Retrieve the textures mip height of a specific level.
		/// </summary>
		/// <param name="mipLevel">Mip level.</param>
		/// <returns>Mip height.</returns>
		uint32_t GetMipHeight(uint32_t mipLevel) const;
		/// <summary>
		/// Retrieve the textures mip size of a specific level.
		/// </summary>
		/// <param name="mipLevel">Mip level.</param>
		/// <returns>Mip size.</returns>
		Math::Vec2ui GetMipSize(uint32_t mipLevel) const;

		/// <summary>
		/// Retrieve the base texture.
		/// </summary>
		/// <returns>Base texture.</returns>
		TRAP::Ref<TRAP::Graphics::TextureBase> GetTexture() const;

		/// <summary>
		/// Update the texture with raw pixel data.
		///
		/// Note: Data array length and sizeInBytes must match the textures current size or it won't update
		/// </summary>
		/// <param name="data">Raw pixel data.</param>
		/// <param name="sizeInBytes">Size of the data array in bytes.</param>
		/// <param name="mipLevel">Mip level to update. Default: 0</param>
		/// <param name="arrayLayer">Array layer to update. Default: 0</param>
		virtual void Update(const void* data, uint32_t sizeInBytes, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) = 0;

		/// <summary>
		/// Check if texture finished loading.
		/// </summary>
		/// <returns>True if texture finished loading, false otherwise.</returns>
		bool IsLoaded() const;
		/// <summary>
		/// Wait for texture to finish loading.
		/// </summary>
		void AwaitLoading() const;

		/// <summary>
		/// Calculate the size of a mip level.
		/// </summary>
		/// <param name="width">Width of the texture.</param>
		/// <param name="height">Height of the texture.</param>
		/// <returns>Size of the mip level.</returns>
		static uint32_t CalculateMipLevels(uint32_t width, uint32_t height);
		/// <summary>
		/// Split a horizontal or vertical cross texture into multiple textures.
		/// </summary>
		/// <param name="image">Image to split.</param>
		/// <returns>Array of splitted textures.</returns>
		template<typename T>
		static std::array<TRAP::Scope<TRAP::Image>, 6> SplitImageFromCross(const TRAP::Scope<TRAP::Image>& image);
	protected:
		/// <summary>
		/// Convert color format and bits per pixel to image format.
		/// </summary>
		/// <param name="colorFormat">Color format.</param>
		/// <param name="bpp">Bits per pixel.</param>
		/// <returns>Image format.</returns>
		static API::ImageFormat ColorFormatBitsPerPixelToImageFormat(Image::ColorFormat colorFormat, uint32_t bpp);
		/// <summary>
		/// Convert image format to color format.
		/// </summary>
		/// <param name="imageFormat">Image format.</param>
		/// <returns>Color format.</returns>
		static Image::ColorFormat ImageFormatToColorFormat(API::ImageFormat imageFormat);
		/// <summary>
		/// Retrieve bits per channel from image format.
		/// </summary>
		/// <param name="imageFormat">Image format.</param>
		/// <returns>Bits per channel.</returns>
		static uint32_t GetBitsPerChannelFromImageFormat(API::ImageFormat imageFormat);
		/// <summary>
		/// Rotate image 90 degrees clockwise.
		/// </summary>
		/// <param name="img">Image to rotate.</param>
		/// <returns>Rotated image.</returns>
		static TRAP::Scope<TRAP::Image> Rotate90Clockwise(const TRAP::Scope<TRAP::Image>& img);
		/// <summary>
		/// Rotate image 90 degrees counter clockwise.
		/// </summary>
		/// <param name="img">Image to rotate.</param>
		/// <returns>Rotated image.</returns>
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