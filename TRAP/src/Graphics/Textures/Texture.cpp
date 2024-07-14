#include "TRAPPCH.h"
#include "Texture.h"

#include "Application.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"
#include "Graphics/API/Objects/Queue.h"

namespace
{
	void AddPathsToHotReloading(const std::span<const std::filesystem::path> paths)
	{
		const auto hotReloadingFileSystemWatcher = TRAP::Application::GetHotReloadingFileSystemWatcher();
		if(!hotReloadingFileSystemWatcher)
			return;

		for(const std::filesystem::path& path : paths)
		{
			if(path.empty())
				continue;

			const auto folderPath = TRAP::FileSystem::GetFolderPath(path);
			if(!folderPath)
				continue;

			hotReloadingFileSystemWatcher->get().AddFolder(*folderPath);
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void AddPathToHotReloading(const std::filesystem::path& path)
	{
		const auto hotReloadingFileSystemWatcher = TRAP::Application::GetHotReloadingFileSystemWatcher();
		if(!hotReloadingFileSystemWatcher)
			return;

		if(path.empty())
			return;

		const auto folderPath = TRAP::FileSystem::GetFolderPath(path);
		if(!folderPath)
			return;

		hotReloadingFileSystemWatcher->get().AddFolder(*folderPath);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Validate texture limits.
	/// @param desc Texture description.
	/// @return True if texture is inside limits, false otherwise.
	[[nodiscard]] bool ValidateLimits(const TRAP::Graphics::RendererAPI::TextureDesc& desc)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

		if(desc.SampleCount > TRAP::Graphics::RendererAPI::SampleCount::One && desc.MipLevels > 1)
		{
			TP_ERROR(TRAP::Log::TexturePrefix, "Multi-Sampled textures cannot have mip maps!");
			TRAP_ASSERT(false, "Texture::ValidateLimits(): Multi-Sampled textures cannot have mip maps!");
			return false;
		}
		const bool cubeMapRequired = (desc.Descriptors & TRAP::Graphics::RendererAPI::DescriptorType::TextureCube) ==
									 TRAP::Graphics::RendererAPI::DescriptorType::TextureCube;
		if(!cubeMapRequired)
		{
			if(desc.Width > TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimension2D)
			{
				TP_ERROR(TRAP::Log::TexturePrefix, "Texture width: ", desc.Width,
						" is bigger than max allowed size: ", TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimension2D, "!");
				TRAP_ASSERT(false, "Texture::ValidateLimits(): Texture width is bigger than max allowed size!");
				return false;
			}
			if(desc.Height > TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimension2D)
			{
				TP_ERROR(TRAP::Log::TexturePrefix, "Texture height: ", desc.Width,
						" is bigger than max allowed size: ", TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimension2D, "!");
				TRAP_ASSERT(false, "Texture::ValidateLimits(): Texture height is bigger than max allowed size!");
				return false;
			}
		}
		else
		{
			if(desc.Width > TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimensionCube)
			{
				TP_ERROR(TRAP::Log::TexturePrefix, "Texture width: ", desc.Width,
						" is bigger than max allowed size: ", TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimensionCube, "!");
				TRAP_ASSERT(false, "Texture::ValidateLimits(): Texture width is bigger than max allowed size!");
				return false;
			}
			if(desc.Height > TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimensionCube)
			{
				TP_ERROR(TRAP::Log::TexturePrefix, "Texture height: ", desc.Width,
						" is bigger than max allowed size: ", TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimensionCube, "!");
				TRAP_ASSERT(false, "Texture::ValidateLimits(): Texture height is bigger than max allowed size!");
				return false;
			}
		}

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert color format and bits per pixel to image format.
	/// @param colorFormat Color format.
	/// @param bpp Bits per pixel.
	/// @return Image format.
	[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat ColorFormatBitsPerPixelToImageFormat(const TRAP::Image::ColorFormat colorFormat,
																						          const u32 bpp)
	{
		switch(colorFormat)
		{
		case TRAP::Image::ColorFormat::GrayScale:
		{
			if(bpp == 8)
				return TRAP::Graphics::API::ImageFormat::R8_UNORM;
			if(bpp == 16)
				return TRAP::Graphics::API::ImageFormat::R16_UNORM;
			if(bpp == 32)
				return TRAP::Graphics::API::ImageFormat::R32_SFLOAT;

			break;
		}

		case TRAP::Image::ColorFormat::GrayScaleAlpha:
		{
			if(bpp == 16)
				return TRAP::Graphics::API::ImageFormat::R8G8_UNORM;
			if(bpp == 32)
				return TRAP::Graphics::API::ImageFormat::R16G16_UNORM;
			if(bpp == 64)
				return TRAP::Graphics::API::ImageFormat::R32G32_SFLOAT;

			break;
		}

		case TRAP::Image::ColorFormat::RGB:
		{
			TRAP_ASSERT(false, "Texture::ColorFormatBitsPerPixelToImageFormat(): Color format RGB is not allowed on empty textures as GPU needs an alpha channel!");
			return TRAP::Graphics::API::ImageFormat::Undefined;
		}

		case TRAP::Image::ColorFormat::RGBA:
		{
			if(bpp == 32)
				return TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
			if(bpp == 64)
				return TRAP::Graphics::API::ImageFormat::R16G16B16A16_UNORM;
			if(bpp == 128)
				return TRAP::Graphics::API::ImageFormat::R32G32B32A32_SFLOAT;

			break;
		}

		case TRAP::Image::ColorFormat::NONE:
			break;
		}

		TRAP_ASSERT(false, "Texture::ColorFormatBitsPerPixelToImageFormat(): Invalid bits per pixel & color format combination provided!");
		return TRAP::Graphics::API::ImageFormat::Undefined;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void UpdateTextureData(TRAP::Graphics::Texture& texture, TRAP::Graphics::API::SyncToken& syncToken,
	                       const std::span<const u8> data, const u32 mipLevel, const u32 arrayLayer)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

		TRAP_ASSERT(arrayLayer < texture.GetArraySize(), "Texture::Update(): Invalid array layer provided!");
		TRAP_ASSERT(mipLevel < texture.GetMipLevels(), "Texture::Update(): Invalid mip level provided!");
		TRAP_ASSERT(data.size() >= (texture.GetWidth() >> mipLevel) * (texture.GetHeight() >> mipLevel) * texture.GetBytesPerPixel(),
					"Texture::Update(): Texture update size is too small");

		if(mipLevel >= texture.GetMipLevels())
		{
			TP_ERROR(TRAP::Log::TexturePrefix, "Update: Invalid mip level provided!");
			return;
		}
		if(data.size() < (texture.GetWidth() >> mipLevel) * (texture.GetHeight() >> mipLevel) * texture.GetBytesPerPixel())
		{
			TP_ERROR(TRAP::Log::TexturePrefix, "Update: Texture update size is too small!");
			return;
		}

		TRAP::Graphics::RendererAPI::TextureUpdateDesc updateDesc{};
		updateDesc.Texture = &texture;
		updateDesc.MipLevel = mipLevel;
		updateDesc.ArrayLayer = arrayLayer;
		TRAP::Graphics::API::ResourceLoader::BeginUpdateResource(updateDesc);
		if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single copy is enough
			std::copy_n(data.begin(), updateDesc.RowCount * updateDesc.SrcRowStride,
						updateDesc.MappedData.begin());
		else //Needs row by row copy
		{
			for(usize r = 0; r < updateDesc.RowCount; ++r)
			{
				std::copy_n(data.begin() + r * updateDesc.SrcRowStride,
							updateDesc.SrcRowStride,
							updateDesc.MappedData.subspan(r * updateDesc.DstRowStride).begin());
			}
		}
		TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &syncToken);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateCube(std::string name,
																			         const std::span<const std::filesystem::path, 6> filepaths,
																			         const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::CreateCube(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::vector<std::filesystem::path>{filepaths.begin(), filepaths.end()});
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateCube(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		AddPathsToHotReloading(filepaths);

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc
		{
			.Texture = texture.get(),
			.Filepaths = texture->m_filepaths,
			.CreationFlag = flags,
			.IsCubemap = true
		};

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateCube(std::string name,
																			         const std::span<const Image*, 6> images,
																			         const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(std::ranges::none_of(images, [](const Image* const img) { return img == nullptr; }),
	            "Texture::CreateCube(): An Image is nullptr!");

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::CreateCube(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	std::vector<std::filesystem::path> imagePaths(images.size());
	for(usize i = 0; i < images.size(); ++i)
	{
		if(images[i] != nullptr)
			imagePaths[i] = images[i]->GetFilePath();
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::move(imagePaths));
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateCube(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		AddPathsToHotReloading(texture->m_filepaths);

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc
		{
			.Texture = texture.get(),
			.Filepaths = {},
			.Images =  {images.begin(), images.end()},
			.CreationFlag = flags,
			.IsCubemap = true
		};

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateCube(std::string name,
																		             const std::filesystem::path& filepath,
																		             const TextureCubeFormat cubeFormat,
																		             const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::CreateCube(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::vector{filepath}, cubeFormat);
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateCube(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		AddPathToHotReloading(filepath);

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc
		{
			.Texture = texture.get(),
			.Filepaths = texture->m_filepaths,
			.CreationFlag = flags,
			.IsCubemap = true,
			.Type = cubeFormat
		};

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateCube(std::string name,
																			         const Image& image,
																					 const TextureCubeFormat cubeFormat,
																			         const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::CreateCube(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::vector{image.GetFilePath()}, cubeFormat);
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateCube(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		AddPathToHotReloading(image.GetFilePath());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc
		{
			.Texture = texture.get(),
			.Filepaths = {},
			.Images = {&image},
			.CreationFlag = flags,
			.IsCubemap = true,
			.Type = cubeFormat
		};

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateCube(std::string name,
																		             const u32 width,
																		             const u32 height,
																		             const u32 bitsPerPixel,
																		             const Image::ColorFormat format,
																		             const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::CreateCube(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	const API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(format, bitsPerPixel);
	if(imageFormat == API::ImageFormat::Undefined)
		return nullptr;

	TRAP::Ref<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(name);
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateCube(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		RendererAPI::TextureDesc texDesc
		{
			.Width = width,
			.Height = height,
			.ArraySize = 6,
			.Format = imageFormat,
			.StartState = RendererAPI::ResourceState::Common,
			.Descriptors = (RendererAPI::DescriptorType::Texture | RendererAPI::DescriptorType::TextureCube),
			.Name = std::move(name)
		};

		if((flags & TextureCreationFlags::Storage) != TextureCreationFlags::None)
			texDesc.Descriptors |= RendererAPI::DescriptorType::RWTexture;

		TRAP::Graphics::RendererAPI::TextureLoadDesc desc
		{
			.Texture = texture.get(),
			.Desc = &texDesc,
			.Filepaths = {},
		};

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::Create2D(std::string name,
																		           const std::filesystem::path& filepath,
																		           const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::Create2D(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::vector{filepath});
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::Create2D(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		AddPathToHotReloading(filepath);

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc
		{
			.Texture = texture.get(),
			.Filepaths = texture->m_filepaths,
			.CreationFlag = flags,
			.IsCubemap = false,
		};

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::Create2D(std::string name,
																			       const TRAP::Image& image,
 																	               const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::Create2D(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::vector{image.GetFilePath()});
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::Create2D(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		AddPathToHotReloading(image.GetFilePath());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc
		{
			.Texture = texture.get(),
			.Filepaths = {},
			.Images = {&image},
			.CreationFlag = flags,
			.IsCubemap = false
		};

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::Create2D(std::string name,
																		           const u32 width,
																		           const u32 height,
																		           const u32 bitsPerPixel,
																		           const Image::ColorFormat format,
																		           const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::Create2D(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	const API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(format, bitsPerPixel);
	if(imageFormat == API::ImageFormat::Undefined)
		return nullptr;

	TRAP::Ref<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(name);
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::Create2D(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		RendererAPI::TextureDesc texDesc
		{
			.Width = width,
			.Height = height,
			.ArraySize = 1,
			.Format = imageFormat,
			.StartState = RendererAPI::ResourceState::Common,
			.Descriptors = RendererAPI::DescriptorType::Texture,
			.Name = std::move(name)
		};

		if((flags & TextureCreationFlags::Storage) != TextureCreationFlags::None)
			texDesc.Descriptors |= RendererAPI::DescriptorType::RWTexture;

		TRAP::Graphics::RendererAPI::TextureLoadDesc desc
		{
			.Texture = texture.get(),
			.Desc = &texDesc,
			.Filepaths = {}
		};

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateCustom(const TRAP::Graphics::RendererAPI::TextureDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    TRAP_ASSERT(desc.Width && desc.Height && (desc.Depth || desc.ArraySize), "Texture::CreateCustom(): Invalid TextureDesc!");

	if(!ValidateLimits(desc))
		return nullptr;

	TRAP::Ref<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(desc.Name);
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateCustom(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		TRAP::Graphics::RendererAPI::TextureLoadDesc loadDesc{};
		loadDesc.Texture = texture.get();

		TRAP::Graphics::RendererAPI::TextureDesc texDesc = desc;
		loadDesc.Desc = &texDesc;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(loadDesc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFallback2D()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	const auto fallbackImg = TRAP::Image::LoadFallback();
	const TRAP::Ref<TRAP::Graphics::Texture> fallback2DTex = Create2D("Fallback2D", *fallbackImg,
	                                                                  TextureCreationFlags::Storage);
	fallback2DTex->AwaitLoading();

	//By default Storage texture are in Unordered Access layout.
	RendererAPI::Transition(fallback2DTex, RendererAPI::ResourceState::UnorderedAccess,
	                        RendererAPI::ResourceState::ShaderResource);

	return fallback2DTex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFallbackCube()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	const auto img = TRAP::Image::LoadFallback();

	std::array<const TRAP::Image*, 6> imgPtrs{};
	for(auto& imgPtr : imgPtrs)
		imgPtr = img.get();

	const TRAP::Ref<TRAP::Graphics::Texture> fallbackCubeTex = CreateCube("FallbackCube", imgPtrs,
	                                                                      TextureCreationFlags::Storage);
	fallbackCubeTex->AwaitLoading();

	//By default Storage texture are in Unordered Access layout.
	RendererAPI::Transition(fallbackCubeTex, RendererAPI::ResourceState::UnorderedAccess,
	                        RendererAPI::ResourceState::ShaderResource);

	return fallbackCubeTex;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Texture::Reload()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	//Can't reload if there is no filepath
	for(const auto& path : m_filepaths)
	{
		if(path.empty())
			return false;
	}

	//Make sure rendering using the texture finished before reloading it.
	RendererAPI::GetGraphicsQueue()->WaitQueueIdle();
	RendererAPI::GetComputeQueue()->WaitQueueIdle();

	//Shutdown the current texture
	Shutdown();

	//Load texture
	TRAP::Graphics::RendererAPI::TextureLoadDesc desc
	{
		.Texture = this,
		.Filepaths = m_filepaths,
		.IsCubemap = GetType() == TextureType::TextureCube,
		.Type = m_textureCubeFormat
	};

	if((m_descriptorTypes & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined)
		desc.CreationFlag |= RendererAPI::TextureCreationFlags::Storage;

	TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &m_syncToken);

	AwaitLoading();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::Update(const std::span<const u8> data, const u32 mipLevel, const u32 arrayLayer)
{
	UpdateTextureData(*this, m_syncToken, data, mipLevel, arrayLayer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::Update(const std::span<const u16> data, const u32 mipLevel, const u32 arrayLayer)
{
	UpdateTextureData(*this, m_syncToken, TRAP::Utils::AsBytes(data), mipLevel, arrayLayer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::Update(const std::span<const f32> data, const u32 mipLevel, const u32 arrayLayer)
{
	UpdateTextureData(*this, m_syncToken, TRAP::Utils::AsBytes(data), mipLevel, arrayLayer);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Texture::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return true; //We don't have a valid sync token, so we assume the texture is loaded

	return RendererAPI::GetResourceLoader()->IsTokenCompleted(m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return; //We don't have a valid sync token, so we assume the texture is loaded

	RendererAPI::GetResourceLoader()->WaitForToken(m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::Texture(std::string name, std::vector<std::filesystem::path> filePaths)
	: m_name(std::move(name)), m_filepaths(std::move(filePaths))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(m_filepaths.size() == 6)
		m_descriptorTypes |= RendererAPI::DescriptorType::TextureCube;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::Texture(std::string name, std::vector<std::filesystem::path> filePaths,
                                 const TRAP::Optional<TextureCubeFormat>& cubeFormat)
	: m_name(std::move(name)), m_filepaths(std::move(filePaths)), m_textureCubeFormat(cubeFormat)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(cubeFormat || m_filepaths.size() == 6)
		m_descriptorTypes |= RendererAPI::DescriptorType::TextureCube;
}
