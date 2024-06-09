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
		if(colorFormat == TRAP::Image::ColorFormat::RGBA)
		{
			if(bpp == 32)
				return TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
			if(bpp == 64)
				return TRAP::Graphics::API::ImageFormat::R16G16B16A16_UNORM;
			if(bpp == 128)
				return TRAP::Graphics::API::ImageFormat::R32G32B32A32_SFLOAT;
		}
		else if(colorFormat == TRAP::Image::ColorFormat::GrayScaleAlpha)
		{
			if(bpp == 16)
				return TRAP::Graphics::API::ImageFormat::R8G8_UNORM;
			if(bpp == 32)
				return TRAP::Graphics::API::ImageFormat::R16G16_UNORM;
			if(bpp == 64)
				return TRAP::Graphics::API::ImageFormat::R32G32_SFLOAT;
		}
		else if(colorFormat == TRAP::Image::ColorFormat::GrayScale)
		{
			if(bpp == 8)
				return TRAP::Graphics::API::ImageFormat::R8_UNORM;
			if(bpp == 16)
				return TRAP::Graphics::API::ImageFormat::R16_UNORM;
			if(bpp == 32)
				return TRAP::Graphics::API::ImageFormat::R32_SFLOAT;
		}
		else if(colorFormat == TRAP::Image::ColorFormat::RGB)
		{
			TRAP_ASSERT(false, "Texture::ColorFormatBitsPerPixelToImageFormat(): Color format RGB is not allowed on empty textures as GPU needs an alpha channel!");
			return TRAP::Graphics::API::ImageFormat::Undefined;
		}

		TRAP_ASSERT(false, "Texture::ColorFormatBitsPerPixelToImageFormat(): Invalid bits per pixel & color format combination provided!");
		return TRAP::Graphics::API::ImageFormat::Undefined;
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
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = texture->m_filepaths;
		desc.IsCubemap = true;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

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

	std::vector<std::filesystem::path> imagePaths{};
	for(const auto* const img : images)
		imagePaths.push_back(img->GetFilePath());

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
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Images = {images.begin(), images.end()};
		desc.IsCubemap = true;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

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
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = texture->m_filepaths;
		desc.IsCubemap = true;
		desc.Type = cubeFormat;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

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
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Images = {&image};
		desc.IsCubemap = true;
		desc.Type = cubeFormat;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

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
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Texture = texture.get();

		RendererAPI::TextureDesc texDesc{};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.Format = imageFormat;
		texDesc.ArraySize = 6;
		texDesc.Name = std::move(name);
		texDesc.Descriptors = (RendererAPI::DescriptorType::Texture | RendererAPI::DescriptorType::TextureCube);
		texDesc.StartState = RendererAPI::ResourceState::Common;
		if((flags & TextureCreationFlags::Storage) != TextureCreationFlags::None)
			texDesc.Descriptors |= RendererAPI::DescriptorType::RWTexture;

		desc.Desc = &texDesc;

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
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = texture->m_filepaths;
		desc.IsCubemap = false;
		desc.Type = TRAP::NullOpt;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

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
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Images = {&image};
		desc.IsCubemap = false;
		desc.Type = TRAP::NullOpt;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

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
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Texture = texture.get();

		RendererAPI::TextureDesc texDesc{};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.Format = imageFormat;
		texDesc.ArraySize = 1;
		texDesc.Name = std::move(name);
		texDesc.Descriptors = RendererAPI::DescriptorType::Texture;
		texDesc.StartState = RendererAPI::ResourceState::Common;
		if((flags & TextureCreationFlags::Storage) != TextureCreationFlags::None)
			texDesc.Descriptors |= RendererAPI::DescriptorType::RWTexture;

		desc.Desc = &texDesc;

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
	TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
	desc.Filepaths = m_filepaths;
	desc.IsCubemap = GetType() == TextureType::TextureCube;
	desc.Type = m_textureCubeFormat;
	desc.CreationFlag = RendererAPI::TextureCreationFlags::None;
	desc.Texture = this;

	if((m_descriptorTypes & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined)
		desc.CreationFlag |= RendererAPI::TextureCreationFlags::Storage;

	TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &m_syncToken);

	AwaitLoading();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::Update(const void* const data, const u32 sizeInBytes, const u32 mipLevel,
                                     const u32 arrayLayer)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(data, "Texture::Update(): Data is nullptr!");
	TRAP_ASSERT(arrayLayer < m_arraySize, "Texture::Update(): Invalid array layer provided!");
	TRAP_ASSERT(mipLevel < m_mipLevels, "Texture::Update(): Invalid mip level provided!");
	TRAP_ASSERT(sizeInBytes >= (m_width >> mipLevel) * (m_height >> mipLevel) * GetBytesPerPixel(),
	            "Texture::Update(): Texture update size is too small");

	if(mipLevel >= m_mipLevels)
	{
		TP_ERROR(Log::TexturePrefix, "Update: Invalid mip level provided!");
		return;
	}
	if(sizeInBytes < (m_width >> mipLevel) * (m_height >> mipLevel) * GetBytesPerPixel())
	{
		TP_ERROR(Log::TexturePrefix, "Update: Texture update size is too small!");
		return;
	}

	RendererAPI::TextureUpdateDesc updateDesc{};
	updateDesc.Texture = this;
	updateDesc.MipLevel = mipLevel;
	updateDesc.ArrayLayer = arrayLayer;
	TRAP::Graphics::API::ResourceLoader::BeginUpdateResource(updateDesc);
	if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single copy is enough
		std::copy_n(static_cast<const u8*>(data), updateDesc.RowCount * updateDesc.SrcRowStride,
		            updateDesc.MappedData.begin());
	else //Needs row by row copy
	{
		for(usize r = 0; r < updateDesc.RowCount; ++r)
		{
			std::copy_n(static_cast<const u8*>(data) + r * updateDesc.SrcRowStride,
			            updateDesc.SrcRowStride,
						updateDesc.MappedData.subspan(r * updateDesc.DstRowStride).begin());
		}
	}
	TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Texture::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return true; //We don't have a valid sync token, so we assume the texture is loaded

	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return; //We don't have a valid sync token, so we assume the texture is loaded

	RendererAPI::GetResourceLoader()->WaitForToken(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::Texture::CalculateMipLevels(const u32 width, const u32 height)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return Math::Max(1u, NumericCast<u32>(Math::Floor(Math::Log2(NumericCast<f32>(Math::Max(width, height))))) + 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::Texture(std::string name)
	: m_name(std::move(name))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);
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
