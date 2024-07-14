#ifndef TRAP_TEXTURE_H
#define TRAP_TEXTURE_H

#include "Graphics/API/ResourceLoader.h"
#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	/// @brief Different texture types.
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
		//TODO Actually return fallback texture on error or change this to nullptr.
		/// @brief Create a cube texture from 6 files.
		/// @param name Name for the texture.
		/// @param filePaths File paths of the 6 texture files.
		///                  Order: +X, -X, +Y, -Y, +Z, -Z
		/// @param flags Additional flags. Default: None.
		/// @return Loaded texture on success, Fallback texture if texture loading failed, nullptr otherwise.
		[[nodiscard]] static Ref<Texture> CreateCube(std::string name, std::span<const std::filesystem::path, 6> filePaths,
		                                             TextureCreationFlags flags = TextureCreationFlags::None);
		/// @brief Create a cube texture from 6 TRAP::Images.
		/// @param name Name for the texture.
		/// @param images Images to create the texture from.
		///               Order: +X, -X, +Y, -Y, +Z, -Z
		/// @param flags Additional flags. Default: None.
		/// @return Loaded texture on success, Fallback texture if texture loading failed, nullptr otherwise.
		/// @note The images must be valid till IsLoaded() returns true.
		[[nodiscard]] static Ref<Texture> CreateCube(std::string name, std::span<const Image*, 6> images,
		                                             TextureCreationFlags flags = TextureCreationFlags::None); //TODO Function missing in TextureManager?
		/// @brief Create a cube texture from 1 file.
		/// @param name Name for the texture.
		/// @param filePath File path of the texture file.
		/// @param cubeFormat Format for the cube texture.
		/// @param flags Additional flags. Default: None.
		/// @return Loaded texture on success, Fallback texture if texture loading failed, nullptr otherwise.
		[[nodiscard]] static Ref<Texture> CreateCube(std::string name, const std::filesystem::path& filePath,
		                                             TextureCubeFormat cubeFormat,
													 TextureCreationFlags flags = TextureCreationFlags::None);
		/// @brief Create a cube texture from 1 TRAP::Image.
		/// @param name Name for the texture.
		/// @param image Image to create the texture from.
		/// @param cubeFormat Format for the cube texture.
		/// @param flags Additional flags. Default: None.
		/// @return Loaded texture on success, Fallback texture if texture loading failed, nullptr otherwise.
		/// @note The image must be valid till IsLoaded() returns true.
		[[nodiscard]] static Ref<Texture> CreateCube(std::string name, const Image& image, TextureCubeFormat cubeFormat,
		                                             TextureCreationFlags flags = TextureCreationFlags::None);
		/// @brief Create an empty cube texture.
		/// @param name Name for the texture.
		/// @param width Width for the texture.
		/// @param height Height for the texture.
		/// @param bitsPerPixel Bits per pixel for the texture.
		/// @param format Color format for the texture.
		/// @param flags Additional flags. Default: None.
		/// @return Empty texture on success, nullptr otherwise.
		[[nodiscard]] static Ref<Texture> CreateCube(std::string name, u32 width, u32 height, u32 bitsPerPixel,
		                                             Image::ColorFormat format,
													 TextureCreationFlags flags = TextureCreationFlags::None);

		/// @brief Create a 2d texture from file.
		/// @param name Name for the texture.
		/// @param filePath File path of the texture.
		/// @param flags Additional flags. Default: None.
		/// @return Loaded texture on success, Fallback texture if texture loading failed, nullptr otherwise.
		[[nodiscard]] static Ref<Texture> Create2D(std::string name, const std::filesystem::path& filePath,
		                                           TextureCreationFlags flags = TextureCreationFlags::None);
		/// @brief Create a 2d texture from TRAP::Image.
		/// @param name Name for the texture.
		/// @param image Image to create the texture from.
		/// @param flags Additional flags. Default: None.
		/// @return Loaded texture on success, Fallback texture if texture loading failed, nullptr otherwise.
		/// @note The image must be valid till IsLoaded() returns true.
		[[nodiscard]] static Ref<Texture> Create2D(std::string name, const Image& image,
		                                           TextureCreationFlags flags = TextureCreationFlags::None);
		/// @brief Create an empty 2d texture.
		/// @param name Name for the texture.
		/// @param width Width for the texture.
		/// @param height Height for the texture.
		/// @param bitsPerPixel Bits per pixel for the texture.
		/// @param format Color format for the texture.
		/// @param flags Additional flags. Default: None.
		/// @return Empty texture on success, nullptr otherwise.
		[[nodiscard]] static Ref<Texture> Create2D(std::string name, u32 width, u32 height, u32 bitsPerPixel,
		                                           Image::ColorFormat format,
												   TextureCreationFlags flags = TextureCreationFlags::None);

		/// @brief Create a custom texture.
		/// @param desc Texture description.
		/// @return Create texture on success, nullptr otherwise.
        [[nodiscard]] static Ref<Texture> CreateCustom(const RendererAPI::TextureDesc& desc);
		/// @brief Create the fallback 2D texture.
		/// @return Fallback 2D texture.
		[[nodiscard]] static Ref<Texture> CreateFallback2D();
		/// @brief Create the fallback cube texture.
		/// @return Fallback cube texture.
		[[nodiscard]] static Ref<Texture> CreateFallbackCube();

		/// @brief Destructor.
		virtual ~Texture() = default;
		/// @brief Copy constructor.
		consteval Texture(const Texture&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Texture& operator=(const Texture&) noexcept = delete;
		/// @brief Move constructor.
		Texture(Texture&&) noexcept = default;
		/// @brief Move assignment operator.
		Texture& operator=(Texture&&) noexcept = default;

		/// @brief Initialize the Texture.
		/// @param desc Texture description.
		virtual void Init(const RendererAPI::TextureDesc& desc) = 0;

		/// @brief Reload texture.
		/// @return True on successful reload (valid texture), else (invalid texture) otherwise.
		bool Reload();

		/// @brief Retrieve the name of the texture.
		/// @return Name of the texture.
		[[nodiscard]] constexpr std::string GetName() const noexcept;
		/// @brief Retrieve the texture type.
		/// @return Texture type.
		[[nodiscard]] constexpr TextureType GetType() const noexcept;
		/// @brief Retrieve the texture width.
		/// @return Texture width.
		[[nodiscard]] constexpr u32 GetWidth() const noexcept;
		/// @brief Retrieve the texture height.
		/// @return Texture height.
		[[nodiscard]] constexpr u32 GetHeight() const noexcept;
		/// @brief Retrieve the texture size.
		/// @return Texture size.
		[[nodiscard]] constexpr Math::Vec2ui GetSize() const noexcept;
		/// @brief Retrieve the texture depth.
		/// @return Texture depth.
		[[nodiscard]] constexpr u32 GetDepth() const noexcept;
		/// @brief Retrieve the texture array size.
		/// @return Texture array size.
		[[nodiscard]] constexpr u32 GetArraySize() const noexcept;
		/// @brief Retrieve the textures mip level count.
		/// @return Textures mip level count.
		[[nodiscard]] constexpr u32 GetMipLevels() const noexcept;
		/// @brief Retrieve the textures aspect mask.
		/// Aspect mask specifies which aspects (Color, Depth, Stencil) are included in the texture.
		/// @return Aspect mask.
		[[nodiscard]] constexpr u32 GetAspectMask() const noexcept;
		/// @brief Retrieve the textures color format.
		/// @return Textures color format.
		[[nodiscard]] constexpr Image::ColorFormat GetColorFormat() const noexcept;
		/// @brief Retrieve the textures image format.
		/// @return Image format.
		[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat GetImageFormat() const noexcept;
		/// @brief Retrieve the textures used descriptor types.
		/// @return Used descriptor types.
		[[nodiscard]] constexpr RendererAPI::DescriptorType GetDescriptorTypes() const noexcept;

		/// @brief Retrieve the textures bits per channel.
		/// @return Textures bits per channel.
		[[nodiscard]] constexpr u32 GetBitsPerChannel() const noexcept;
		/// @brief Retrieve the textures bytes per channel.
		/// @return Textures bytes per channel.
		[[nodiscard]] constexpr u32 GetBytesPerChannel() const noexcept;
		/// @brief Retrieve the textures bits per pixel.
		/// @return Textures bits per pixel.
		[[nodiscard]] constexpr u32 GetBitsPerPixel() const noexcept;
		/// @brief Retrieve the textures bytes per pixel.
		/// @return Textures bytes per pixel.
		[[nodiscard]] constexpr u32 GetBytesPerPixel() const noexcept;
		/// @brief Retrieve the textures mip width of a specific level.
		/// @param mipLevel Mip level.
		/// @return Mip width.
		[[nodiscard]] constexpr u32 GetMipWidth(u32 mipLevel) const;
		/// @brief Retrieve the textures mip height of a specific level.
		/// @param mipLevel Mip level.
		/// @return Mip height.
		[[nodiscard]] constexpr u32 GetMipHeight(u32 mipLevel) const;
		/// @brief Retrieve the textures mip size of a specific level.
		/// @param mipLevel Mip level.
		/// @return Mip size.
		[[nodiscard]] constexpr Math::Vec2ui GetMipSize(u32 mipLevel) const;
		/// @brief Retrieve the file paths of the texture.
		/// @return File paths of the texture.
		[[nodiscard]] constexpr const std::vector<std::filesystem::path>& GetFilePaths() const noexcept;
		/// @brief Retrieve the cube format of the texture.
		/// @return Cube format of the texture.
		[[nodiscard]] constexpr TRAP::Optional<TextureCubeFormat> GetCubeFormat() const noexcept;

		/// @brief Update the texture with raw pixel data.
		/// @param data Raw pixel data.
		/// @param mipLevel Mip level to update. Default: 0
		/// @param arrayLayer Array layer to update. Default: 0
		/// @note Data array length and sizeInBytes must match the textures current size or it won't update
		void Update(std::span<const u8>, u32 mipLevel = 0, u32 arrayLayer = 0);
		/// @brief Update the texture with raw pixel data.
		/// @param data Raw pixel data.
		/// @param mipLevel Mip level to update. Default: 0
		/// @param arrayLayer Array layer to update. Default: 0
		/// @note Data array length and sizeInBytes must match the textures current size or it won't update
		void Update(std::span<const u16>, u32 mipLevel = 0, u32 arrayLayer = 0);
		/// @brief Update the texture with raw pixel data.
		/// @param data Raw pixel data.
		/// @param mipLevel Mip level to update. Default: 0
		/// @param arrayLayer Array layer to update. Default: 0
		/// @note Data array length and sizeInBytes must match the textures current size or it won't update
		void Update(std::span<const f32>, u32 mipLevel = 0, u32 arrayLayer = 0);

		/// @brief Retrieve whether the texture owns the image data.
		/// @return True if texture owns the image data, false otherwise.
		[[nodiscard]] constexpr bool OwnsImage() const noexcept;

		/// @brief Check if texture finished loading.
		/// @return True if texture finished loading, false otherwise.
		[[nodiscard]] bool IsLoaded() const;
		/// @brief Wait for texture to finish loading.
		void AwaitLoading() const;

		/// @brief Calculate the size of a mip level.
		/// @param width Width of the texture.
		/// @param height Height of the texture.
		/// @return Size of the mip level.
		[[nodiscard]] static constexpr u32 CalculateMipLevels(u32 width, u32 height);
		/// @brief Split a horizontal or vertical cross texture into multiple textures.
		/// @param image Image to split.
		/// @return Array of splitted textures.
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static std::array<TRAP::Scope<TRAP::Image>, 6> SplitImageFromCross(const TRAP::Image& image);

	protected:
		/// @brief Shutdown API dependent texture.
		virtual void Shutdown() = 0;

		/// @brief Convert image format to color format.
		/// @param imageFormat Image format.
		/// @return Color format.
		[[nodiscard]] static constexpr Image::ColorFormat ImageFormatToColorFormat(API::ImageFormat imageFormat) noexcept;

		/// @brief Retrieve bits per channel from image format.
		/// @param imageFormat Image format.
		/// @return Bits per channel.
		[[nodiscard]] static constexpr u32 GetBitsPerChannelFromImageFormat(API::ImageFormat imageFormat) noexcept;

		/// @brief Constructor.
		constexpr explicit Texture(std::string name);
		/// @brief Constructor.
		Texture(std::string name, std::vector<std::filesystem::path> filePaths);
		/// @brief Constructor.
		Texture(std::string name, std::vector<std::filesystem::path> filePaths, const TRAP::Optional<TextureCubeFormat>& cubeFormat);

		//RenderAPI independent data
		std::string m_name;
		API::SyncToken m_syncToken = 0;
		u32 m_width = 2;
		u32 m_height = 2;
		u32 m_depth = 1;
		u32 m_arraySize = 1;
		u32 m_mipLevels = 1;
		Graphics::API::ImageFormat m_imageFormat = Graphics::API::ImageFormat::R8G8B8A8_UNORM;
		u32 m_aspectMask = 0;
		RendererAPI::DescriptorType m_descriptorTypes = RendererAPI::DescriptorType::Texture;
		bool m_ownsImage = true;
		std::vector<std::filesystem::path> m_filepaths;
		TRAP::Optional<TextureCubeFormat> m_textureCubeFormat = TRAP::NullOpt;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Graphics::Texture::GetName() const noexcept
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::TextureType TRAP::Graphics::Texture::GetType() const noexcept
{
	if((m_descriptorTypes & RendererAPI::DescriptorType::TextureCube) == RendererAPI::DescriptorType::TextureCube)
		return TextureType::TextureCube;

	return TextureType::Texture2D;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetHeight() const noexcept
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2ui TRAP::Graphics::Texture::GetSize() const noexcept
{
	return Math::Vec2ui(m_width, m_height);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetDepth() const noexcept
{
	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetArraySize() const noexcept
{
	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetMipLevels() const noexcept
{
	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetAspectMask() const noexcept
{
	return m_aspectMask;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Image::ColorFormat TRAP::Graphics::Texture::GetColorFormat() const noexcept
{
	return ImageFormatToColorFormat(m_imageFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat TRAP::Graphics::Texture::GetImageFormat() const noexcept
{
	return m_imageFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::Texture::GetDescriptorTypes() const noexcept
{
	return m_descriptorTypes;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetBitsPerChannel() const noexcept
{
	return GetBitsPerChannelFromImageFormat(m_imageFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetBytesPerChannel() const noexcept
{
	return GetBitsPerChannel() / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetBitsPerPixel() const noexcept
{
	return GetBitsPerChannel() * std::to_underlying(ImageFormatToColorFormat(m_imageFormat));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetBytesPerPixel() const noexcept
{
	return GetBitsPerPixel() / 8u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetMipWidth(const u32 mipLevel) const
{
	if(mipLevel >= m_mipLevels)
		TP_ERROR(Log::TexturePrefix, "GetMipWidth: Invalid mip level provided!");

	return Math::Max(1u, m_width >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetMipHeight(const u32 mipLevel) const
{
	if(mipLevel >= m_mipLevels)
		TP_ERROR(Log::TexturePrefix, "GetMipHeight: Invalid mip level provided!");

	return Math::Max(1u, m_height >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2ui TRAP::Graphics::Texture::GetMipSize(const u32 mipLevel) const
{
	return TRAP::Math::Vec2ui{GetMipWidth(mipLevel), GetMipHeight(mipLevel)};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<std::filesystem::path>& TRAP::Graphics::Texture::GetFilePaths() const noexcept
{
	return m_filepaths;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Optional<TRAP::Graphics::TextureCubeFormat> TRAP::Graphics::Texture::GetCubeFormat() const noexcept
{
	return m_textureCubeFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::Texture::OwnsImage() const noexcept
{
	return m_ownsImage;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::CalculateMipLevels(const u32 width, const u32 height)
{
	return Math::Max(1u, NumericCast<u32>(Math::Floor(Math::Log2(NumericCast<f32>(Math::Max(width, height))))) + 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] std::array<TRAP::Scope<TRAP::Image>, 6> TRAP::Graphics::Texture::SplitImageFromCross(const TRAP::Image& image)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	const bool isHorizontal = image.GetWidth() > image.GetHeight();

	const u32 stride = image.GetBytesPerPixel();
	u32 face = 0;
	u32 cxLimit = 4, cyLimit = 3;
	if(!isHorizontal)
	{
		cxLimit = 3;
		cyLimit = 4;
	}
	const u32 faceWidth = image.GetWidth() / cxLimit;
	const u32 faceHeight = image.GetHeight() / cyLimit;

	std::array<std::vector<T>, 6> cubeTextureData;
	for(auto& i : cubeTextureData)
		i.resize(faceWidth * faceHeight * stride);

	const std::span<const T> pixelData{reinterpret_cast<const T*>(image.GetPixelData().data()), image.GetPixelData().size() / sizeof(T)};

	for(u32 cy = 0; cy < cyLimit; ++cy)
	{
		for(u32 cx = 0; cx < cxLimit; ++cx)
		{
			if((cy == 0 || cy == 2 || cy == 3) && cx != 1)
				continue;

			for(u32 y = 0; y < faceHeight; ++y)
			{
				u32 offset = y;
				if(!isHorizontal && face == 5)
					offset = faceHeight - (y + 1);
				const u32 yp = cy * faceHeight + offset;

				for(u32 x = 0; x < faceWidth; ++x)
				{
					offset = x;
					if(!isHorizontal && face == 5)
						offset = faceWidth - (x + 1);
					const u32 xp = cx * faceWidth + offset;

					if(stride == 1)
					{
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = pixelData[(xp + yp * image.GetWidth()) * stride + 0];
					}
					else if(stride == 2)
					{
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = pixelData[(xp + yp * image.GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = pixelData[(xp + yp * image.GetWidth()) * stride + 1];
					}
					else if(stride == 3)
					{
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = pixelData[(xp + yp * image.GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = pixelData[(xp + yp * image.GetWidth()) * stride + 1];
						cubeTextureData[face][(x + y * faceWidth) * stride + 2] = pixelData[(xp + yp * image.GetWidth()) * stride + 2];
					}
					else if(stride == 4)
					{
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = pixelData[(xp + yp * image.GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = pixelData[(xp + yp * image.GetWidth()) * stride + 1];
						cubeTextureData[face][(x + y * faceWidth) * stride + 2] = pixelData[(xp + yp * image.GetWidth()) * stride + 2];
						cubeTextureData[face][(x + y * faceWidth) * stride + 3] = pixelData[(xp + yp * image.GetWidth()) * stride + 3];
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
		std::get<0>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<3>(cubeTextureData)); //+X
		std::get<1>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<1>(cubeTextureData)); //-X
		std::get<2>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<0>(cubeTextureData)); //+Y
		std::get<3>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<5>(cubeTextureData)); //-Y
		std::get<4>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<2>(cubeTextureData)); //+Z
		std::get<5>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<4>(cubeTextureData)); //-Z
	}
	else
	{
		std::get<0>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<2>(cubeTextureData)); //+X
		std::get<1>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<5>(cubeTextureData)); //-X
		std::get<2>(images) = TRAP::Image::Rotate90CounterClockwise(TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<0>(cubeTextureData)).get()); //+Y
		std::get<3>(images) = TRAP::Image::Rotate90Clockwise(TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<4>(cubeTextureData)).get()); //-Y
		std::get<4>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<1>(cubeTextureData)); //+Z
		std::get<5>(images) = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image.GetColorFormat(), std::get<3>(cubeTextureData)); //-Z
	}

	return images;
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

[[nodiscard]] constexpr u32 TRAP::Graphics::Texture::GetBitsPerChannelFromImageFormat(const API::ImageFormat imageFormat) noexcept
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

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::Texture::Texture(std::string name)
	: m_name(std::move(name))
{
}

#endif /*TRAP_TEXTURE_H*/
