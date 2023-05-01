#ifndef TRAP_TEXTURE_H
#define TRAP_TEXTURE_H

#include "Graphics/API/ResourceLoader.h"
#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	/// <summary>
	/// Different texture types.
	/// </summary>
	enum class TextureType
	{
		Texture2D,
		TextureCube
	};

	using TextureCubeFormat = RendererAPI::TextureCubeType;
	using TextureCreationFlags = RendererAPI::TextureCreationFlags;

	class Texture
	{
	public:
		/// <summary>
		/// Create a cube texture from 6 files.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepaths">
		/// File paths of the 6 texture files.
		/// Order: +X, -X, +Y, -Y, +Z, -Z
		/// </param>
		/// <param name="flags">Additional flags. Default: None.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		[[nodiscard]] static Ref<Texture> CreateFromFiles(std::string name,
		                                                  std::array<std::filesystem::path, 6> filepaths,
											              TextureCreationFlags flags = TextureCreationFlags::None);
		/// <summary>
		/// Create a texture from file.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepath">File path of the texture.</param>
		/// <param name="type">Type of Texture.</param>
		/// <param name="cubeFormat">Format of the cube texture. Ignored when using TextureType::Texture2D.</param>
		/// <param name="flags">Additional flags. Default: None.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		[[nodiscard]] static Ref<Texture> CreateFromFile(std::string name, std::filesystem::path filepath, TextureType type,
		                                                 TextureCubeFormat cubeFormat = TextureCubeFormat::NONE,
										                 TextureCreationFlags flags = TextureCreationFlags::None);
		/// <summary>
		/// Create a texture from file.
		/// File name will be used as the texture name.
		/// </summary>
		/// <param name="filepath">File path of the texture.</param>
		/// <param name="type">Type of Texture.</param>
		/// <param name="cubeFormat">Format of the cube texture. Ignored when using TextureType::Texture2D.</param>
		/// <param name="flags">Additional flags. Default: None.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		[[nodiscard]] static Ref<Texture> CreateFromFile(std::filesystem::path filepath, TextureType type,
		                                                 TextureCubeFormat cubeFormat = TextureCubeFormat::NONE,
										                 TextureCreationFlags flags = TextureCreationFlags::None);
		/// <summary>
		/// Create a cube texture from 6 TRAP::Images.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="imgs">
		/// Images to create the texture from.
		/// Order: +X, -X, +Y, -Y, +Z, -Z
		/// Note: The images must be valid till IsLoaded() returns true.
		/// </param>
		/// <param name="flags">Additional flags. Default: None.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		[[nodiscard]] static Ref<Texture> CreateFromImages(std::string name, const std::array<const Image*, 6>& imgs,
											               TextureCreationFlags flags = TextureCreationFlags::None);
		/// <summary>
		/// Create a texture from TRAP::Image.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="img">
		/// Image to create the texture from.
		/// Note: The image must be valid till IsLoaded() returns true.
		/// </param>
		/// <param name="type">Type of Texture.</param>
		/// <param name="cubeFormat">Format of the cube texture. Ignored when using TextureType::Texture2D.</param>
		/// <param name="flags">Additional flags. Default: None.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		[[nodiscard]] static Ref<Texture> CreateFromImage(std::string name, const TRAP::Image* img, TextureType type,
		                                                  TextureCubeFormat cubeFormat = TextureCubeFormat::NONE,
											              TextureCreationFlags flags = TextureCreationFlags::None);
		/// <summary>
		/// Create an empty texture.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="width">Width for the texture.</param>
		/// <param name="height">Height for the texture.</param>
		/// <param name="bitsPerPixel">Bits per pixel for the texture.</param>
		/// <param name="format">Color format for the texture.</param>
		/// <param name="type">Type of texture.</param>
		/// <param name="flags">Additional flags. Default: None.</param>
		/// <returns>Empty texture on success, nullptr otherwise.</returns>
		[[nodiscard]] static Ref<Texture> CreateEmpty(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel,
		                                              Image::ColorFormat format, TextureType type,
										              TextureCreationFlags flags = TextureCreationFlags::None);
		/// <summary>
		/// Create a custom texture.
		/// </summary>
		/// <param name="desc">Texture description.</param>
		/// <returns>New texture.</returns>
        [[nodiscard]] static Ref<Texture> CreateCustom(const RendererAPI::TextureDesc& desc);
		/// <summary>
		/// Create the fallback 2D texture.
		/// </summary>
		/// <returns>Fallback 2D texture.</returns>
		[[nodiscard]] static Ref<Texture> CreateFallback2D();
		/// <summary>
		/// Create the fallback cube texture.
		/// </summary>
		/// <returns>Fallback cube texture.</returns>
		[[nodiscard]] static Ref<Texture> CreateFallbackCube();

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Texture() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		Texture(const Texture&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Texture& operator=(const Texture&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Texture(Texture&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Texture& operator=(Texture&&) noexcept = default;

		/// <summary>
		/// Initialize the Texture.
		/// </summary>
		/// <param name="desc">Texture description.</param>
		virtual void Init(const RendererAPI::TextureDesc& desc) = 0;

		/// <summary>
		/// Reload texture.
		/// </summary>
		/// <returns>True on successful reload (valid texture), else (invalid texture) otherwise.</returns>
		bool Reload();

		/// <summary>
		/// Retrieve the name of the texture.
		/// </summary>
		/// <returns>Name of the texture.</returns>
		[[nodiscard]] std::string GetName() const noexcept;
		/// <summary>
		/// Retrieve the texture type.
		/// </summary>
		/// <returns>Texture type.</returns>
		[[nodiscard]] TextureType GetType() const noexcept;
		/// <summary>
		/// Retrieve the texture width.
		/// </summary>
		/// <returns>Texture width.</returns>
		[[nodiscard]] uint32_t GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the texture height.
		/// </summary>
		/// <returns>Texture height.</returns>
		[[nodiscard]] uint32_t GetHeight() const noexcept;
		/// <summary>
		/// Retrieve the texture size.
		/// </summary>
		/// <returns>Texture size.</returns>
		[[nodiscard]] Math::Vec2ui GetSize() const noexcept;
		/// <summary>
		/// Retrieve the texture depth.
		/// </summary>
		/// <returns>Texture depth.</returns>
		[[nodiscard]] uint32_t GetDepth() const noexcept;
		/// <summary>
		/// Retrieve the texture array size.
		/// </summary>
		/// <returns>Texture array size.</returns>
		[[nodiscard]] uint32_t GetArraySize() const noexcept;
		/// <summary>
		/// Retrieve the textures mip level count.
		/// </summary>
		/// <returns>Textures mip level count.</returns>
		[[nodiscard]] uint32_t GetMipLevels() const noexcept;
		/// <summary>
		/// Retrieve the textures aspect mask.
		/// Aspect mask specifies which aspects (Color, Depth, Stencil) are included in the texture.
		/// </summary>
		/// <returns>Aspect mask.</returns>
		[[nodiscard]] uint32_t GetAspectMask() const noexcept;
		/// <summary>
		/// Retrieve the textures color format.
		/// </summary>
		/// <returns>Textures color format.</returns>
		[[nodiscard]] Image::ColorFormat GetColorFormat() const noexcept;
		/// <summary>
		/// Retrieve the textures image format.
		/// </summary>
		/// <returns>Image format.</returns>
		[[nodiscard]] TRAP::Graphics::API::ImageFormat GetImageFormat() const noexcept;
		/// <summary>
		/// Retrieve the textures used descriptor types.
		/// </summary>
		/// <returns>Used descriptor types.</returns>
		[[nodiscard]] RendererAPI::DescriptorType GetDescriptorTypes() const noexcept;

		/// <summary>
		/// Retrieve the textures bits per channel.
		/// </summary>
		/// <returns>Textures bits per channel.</returns>
		[[nodiscard]] uint32_t GetBitsPerChannel() const noexcept;
		/// <summary>
		/// Retrieve the textures bytes per channel.
		/// </summary>
		/// <returns>Textures bytes per channel.</returns>
		[[nodiscard]] uint32_t GetBytesPerChannel() const noexcept;
		/// <summary>
		/// Retrieve the textures bits per pixel.
		/// </summary>
		/// <returns>Textures bits per pixel.</returns>
		[[nodiscard]] uint32_t GetBitsPerPixel() const noexcept;
		/// <summary>
		/// Retrieve the textures bytes per pixel.
		/// </summary>
		/// <returns>Textures bytes per pixel.</returns>
		[[nodiscard]] uint32_t GetBytesPerPixel() const noexcept;
		/// <summary>
		/// Retrieve the textures mip width of a specific level.
		/// </summary>
		/// <param name="mipLevel">Mip level.</param>
		/// <returns>Mip width.</returns>
		[[nodiscard]] uint32_t GetMipWidth(uint32_t mipLevel) const;
		/// <summary>
		/// Retrieve the textures mip height of a specific level.
		/// </summary>
		/// <param name="mipLevel">Mip level.</param>
		/// <returns>Mip height.</returns>
		[[nodiscard]] uint32_t GetMipHeight(uint32_t mipLevel) const;
		/// <summary>
		/// Retrieve the textures mip size of a specific level.
		/// </summary>
		/// <param name="mipLevel">Mip level.</param>
		/// <returns>Mip size.</returns>
		[[nodiscard]] Math::Vec2ui GetMipSize(uint32_t mipLevel) const;
		/// <summary>
		/// Retrieve the file path of the texture.
		/// </summary>
		/// <returns>File path of the texture.</returns>
		[[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept;
		/// <summary>
		/// Retrieve the file path of the texture.
		/// </summary>
		/// <returns>File path of the texture.</returns>
		[[nodiscard]] const std::array<std::filesystem::path, 6>& GetFilePaths() const noexcept;
		/// <summary>
		/// Retrieve the cube format of the texture.
		/// </summary>
		/// <returns>Cube format of the texture.</returns>
		[[nodiscard]] TextureCubeFormat GetCubeFormat() const noexcept;

		/// <summary>
		/// Update the texture with raw pixel data.
		///
		/// Note: Data array length and sizeInBytes must match the textures current size or it won't update
		/// </summary>
		/// <param name="data">Raw pixel data.</param>
		/// <param name="sizeInBytes">Size of the data array in bytes.</param>
		/// <param name="mipLevel">Mip level to update. Default: 0</param>
		/// <param name="arrayLayer">Array layer to update. Default: 0</param>
		void Update(const void* data, uint32_t sizeInBytes, uint32_t mipLevel = 0, uint32_t arrayLayer = 0);

		/// <summary>
		/// Set the texture name.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
        virtual void SetTextureName(std::string_view name) const = 0;

		/// <summary>
		/// Retrieve whether the texture owns the image data.
		/// </summary>
		/// <returns>True if texture owns the image data, false otherwise.</returns>
		[[nodiscard]] bool OwnsImage() const noexcept;

		/// <summary>
		/// Check if texture finished loading.
		/// </summary>
		/// <returns>True if texture finished loading, false otherwise.</returns>
		[[nodiscard]] bool IsLoaded() const;
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
		[[nodiscard]] static uint32_t CalculateMipLevels(uint32_t width, uint32_t height);
		/// <summary>
		/// Split a horizontal or vertical cross texture into multiple textures.
		/// </summary>
		/// <param name="image">Image to split.</param>
		/// <returns>Array of splitted textures.</returns>
		template<typename T>
		[[nodiscard]] static std::array<TRAP::Scope<TRAP::Image>, 6> SplitImageFromCross(const TRAP::Image* image);

	protected:
		/// <summary>
		/// Shutdown API dependent texture.
		/// </summary>
		virtual void Shutdown() = 0;

		/// <summary>
		/// Validate texture limits.
		/// </summary>
		/// <param name="desc">Texture description.</param>
		/// <returns>True if texture is inside limits, false otherwise.</returns>
		[[nodiscard]] static bool ValidateLimits(const RendererAPI::TextureDesc& desc);

		/// <summary>
		/// Convert color format and bits per pixel to image format.
		/// </summary>
		/// <param name="colorFormat">Color format.</param>
		/// <param name="bpp">Bits per pixel.</param>
		/// <returns>Image format.</returns>
		[[nodiscard]] static constexpr API::ImageFormat ColorFormatBitsPerPixelToImageFormat(Image::ColorFormat colorFormat, uint32_t bpp);
		/// <summary>
		/// Convert image format to color format.
		/// </summary>
		/// <param name="imageFormat">Image format.</param>
		/// <returns>Color format.</returns>
		[[nodiscard]] static constexpr Image::ColorFormat ImageFormatToColorFormat(API::ImageFormat imageFormat) noexcept;
		/// <summary>
		/// Retrieve bits per channel from image format.
		/// </summary>
		/// <param name="imageFormat">Image format.</param>
		/// <returns>Bits per channel.</returns>
		[[nodiscard]] static constexpr uint32_t GetBitsPerChannelFromImageFormat(API::ImageFormat imageFormat) noexcept;

		/// <summary>
		/// Constructor.
		/// </summary>
		Texture() noexcept;

		//RenderAPI independent data
		std::string m_name;
		API::SyncToken m_syncToken;
		TextureType m_textureType;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint32_t m_arraySize;
		uint32_t m_mipLevels;
		Image::ColorFormat m_colorFormat;
		Graphics::API::ImageFormat m_imageFormat;
		uint32_t m_aspectMask;
		RendererAPI::DescriptorType m_descriptorTypes;
		bool m_ownsImage;
		std::array<std::filesystem::path, 6> m_filepaths;
		TextureCubeFormat m_textureCubeFormat;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] std::array<TRAP::Scope<TRAP::Image>, 6> TRAP::Graphics::Texture::SplitImageFromCross(const TRAP::Image* const image)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const bool isHorizontal = image->GetWidth() > image->GetHeight();

	const uint32_t stride = image->GetBytesPerPixel();
	uint32_t face = 0;
	uint32_t cxLimit = 4, cyLimit = 3;
	if(!isHorizontal)
	{
		cxLimit = 3;
		cyLimit = 4;
	}
	const uint32_t faceWidth = image->GetWidth() / cxLimit;
	const uint32_t faceHeight = image->GetHeight() / cyLimit;

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

					if(stride == 1)
					{
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
					}
					else if(stride == 2)
					{
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 1];
					}
					else if(stride == 3)
					{
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
					}
					else if(stride == 4)
					{
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
		std::get<0>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<3>(cubeTextureData)); //+X
		std::get<1>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<1>(cubeTextureData)); //-X
		std::get<2>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<0>(cubeTextureData)); //+Y
		std::get<3>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<5>(cubeTextureData)); //-Y
		std::get<4>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<2>(cubeTextureData)); //+Z
		std::get<5>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<4>(cubeTextureData)); //-Z
	}
	else
	{
		std::get<0>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<2>(cubeTextureData)); //+X
		std::get<1>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<5>(cubeTextureData)); //-X
		// std::get<2>(images) = Rotate90CounterClockwise(TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<0>(cubeTextureData)).get()); //+Y
		// std::get<3>(images) = Rotate90Clockwise(TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<4>(cubeTextureData)).get()); //-Y
		std::get<2>(images) = TRAP::Image::Rotate90CounterClockwise(TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<0>(cubeTextureData)).get()); //+Y
		std::get<3>(images) = TRAP::Image::Rotate90Clockwise(TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<4>(cubeTextureData)).get()); //-Y
		std::get<4>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<1>(cubeTextureData)); //+Z
		std::get<5>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), std::get<3>(cubeTextureData)); //-Z
	}

	return images;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat TRAP::Graphics::Texture::ColorFormatBitsPerPixelToImageFormat(const Image::ColorFormat colorFormat,
	                             											                                           const uint32_t bpp)
{
	if(colorFormat == Image::ColorFormat::GrayScale)
	{
		if(bpp == 8)
			return API::ImageFormat::R8_UNORM;
		if(bpp == 16)
			return API::ImageFormat::R16_UNORM;
		if(bpp == 32)
			return API::ImageFormat::R32_SFLOAT;

		TRAP_ASSERT(false, "Texture::ColorFormatBitsPerPixelToImageFormat(): Invalid bits per pixel & color format combination provided!");
		return API::ImageFormat::Undefined;
	}
	if(colorFormat == Image::ColorFormat::GrayScaleAlpha)
	{
		if(bpp == 16)
			return API::ImageFormat::R8G8_UNORM;
		if(bpp == 32)
		    return API::ImageFormat::R16G16_UNORM;
		if(bpp == 64)
		    return API::ImageFormat::R32G32_SFLOAT;

		TRAP_ASSERT(false, "Texture::ColorFormatBitsPerPixelToImageFormat(): Invalid bits per pixel & color format combination provided!");
		return API::ImageFormat::Undefined;
	}
	if(colorFormat == Image::ColorFormat::RGB)
	{
		TRAP_ASSERT(false, "Texture::ColorFormatBitsPerPixelToImageFormat(): Color format RGB is not allowed on empty textures as GPU needs an alpha channel!");
		return API::ImageFormat::Undefined;
	}
	if(colorFormat == Image::ColorFormat::RGBA)
	{
		if(bpp == 32)
			return API::ImageFormat::R8G8B8A8_UNORM;
		if(bpp == 64)
			return API::ImageFormat::R16G16B16A16_UNORM;
		if(bpp == 128)
			return API::ImageFormat::R32G32B32A32_SFLOAT;

		TRAP_ASSERT(false, "Texture::ColorFormatBitsPerPixelToImageFormat(): Invalid bits per pixel & color format combination provided!");
		return API::ImageFormat::Undefined;
	}

	TRAP_ASSERT(false, "Texture::ColorFormatBitsPerPixelToImageFormat(): Invalid color format provided!");
	return API::ImageFormat::Undefined;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Image::ColorFormat TRAP::Graphics::Texture::ImageFormatToColorFormat(const API::ImageFormat imageFormat) noexcept
{
	switch(imageFormat)
	{
	case API::ImageFormat::R8_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R8_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R8_UINT:
		[[fallthrough]];
	case API::ImageFormat::R8_SINT:
		[[fallthrough]];
	case API::ImageFormat::R8_SRGB:
		[[fallthrough]];
	case API::ImageFormat::R16_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R16_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R16_UINT:
		[[fallthrough]];
	case API::ImageFormat::R16_SINT:
		[[fallthrough]];
	case API::ImageFormat::R16_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R16_SBFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R32_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R32_SINT:
		[[fallthrough]];
	case API::ImageFormat::R32_UINT:
		return Image::ColorFormat::GrayScale;

	case API::ImageFormat::R8G8_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R8G8_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R8G8_UINT:
		[[fallthrough]];
	case API::ImageFormat::R8G8_SINT:
		[[fallthrough]];
	case API::ImageFormat::R8G8_SRGB:
		[[fallthrough]];
	case API::ImageFormat::R16G16_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R16G16_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R16G16_UINT:
		[[fallthrough]];
	case API::ImageFormat::R16G16_SINT:
		[[fallthrough]];
	case API::ImageFormat::R16G16_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R16G16_SBFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R32G32_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R32G32_SINT:
		[[fallthrough]];
	case API::ImageFormat::R32G32_UINT:
		return Image::ColorFormat::GrayScaleAlpha;

	case API::ImageFormat::R8G8B8A8_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R8G8B8A8_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R8G8B8A8_UINT:
		[[fallthrough]];
	case API::ImageFormat::R8G8B8A8_SINT:
		[[fallthrough]];
	case API::ImageFormat::R8G8B8A8_SRGB:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_UINT:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_SINT:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_SBFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R32G32B32A32_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R32G32B32A32_UINT:
		[[fallthrough]];
	case API::ImageFormat::R32G32B32A32_SINT:
		return Image::ColorFormat::RGBA;

	default:
		return Image::ColorFormat::NONE;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::Texture::GetBitsPerChannelFromImageFormat(const API::ImageFormat imageFormat) noexcept
{
	switch(imageFormat)
	{
	case API::ImageFormat::R8_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R8_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R8_UINT:
		[[fallthrough]];
	case API::ImageFormat::R8_SINT:
		[[fallthrough]];
	case API::ImageFormat::R8_SRGB:
		[[fallthrough]];
	case API::ImageFormat::R8G8_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R8G8_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R8G8_UINT:
		[[fallthrough]];
	case API::ImageFormat::R8G8_SINT:
		[[fallthrough]];
	case API::ImageFormat::R8G8_SRGB:
		[[fallthrough]];
	case API::ImageFormat::R8G8B8A8_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R8G8B8A8_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R8G8B8A8_UINT:
		[[fallthrough]];
	case API::ImageFormat::R8G8B8A8_SINT:
		[[fallthrough]];
	case API::ImageFormat::R8G8B8A8_SRGB:
		return 8;

	case API::ImageFormat::R16_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R16_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R16_UINT:
		[[fallthrough]];
	case API::ImageFormat::R16_SINT:
		[[fallthrough]];
	case API::ImageFormat::R16_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R16G16_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R16G16_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R16G16_UINT:
		[[fallthrough]];
	case API::ImageFormat::R16G16_SINT:
		[[fallthrough]];
	case API::ImageFormat::R16G16_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R16G16_SBFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_UNORM:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_SNORM:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_UINT:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_SINT:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R16G16B16A16_SBFLOAT:
		return 16;

	case API::ImageFormat::R32_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R32_SINT:
		[[fallthrough]];
	case API::ImageFormat::R32_UINT:
		[[fallthrough]];
	case API::ImageFormat::R32G32_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R32G32_SINT:
		[[fallthrough]];
	case API::ImageFormat::R32G32_UINT:
		[[fallthrough]];
	case API::ImageFormat::R32G32B32A32_SFLOAT:
		[[fallthrough]];
	case API::ImageFormat::R32G32B32A32_UINT:
		[[fallthrough]];
	case API::ImageFormat::R32G32B32A32_SINT:
		return 32;

	default:
		return 0;
	}
}

#endif /*TRAP_TEXTURE_H*/