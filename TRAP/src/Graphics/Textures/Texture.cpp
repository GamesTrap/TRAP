#include "TRAPPCH.h"
#include "Texture.h"

#include "Application.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"
#include "Graphics/API/Objects/Queue.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromFiles(std::string name,
																			              std::array<std::filesystem::path, 6> filepaths,
																			              const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::CreateFromFiles(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::move(filepaths));

		//Hot Reloading
		const auto hotReloadingFileWatcher = TRAP::Application::GetHotReloadingFileWatcher();

		if(hotReloadingFileWatcher)
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					hotReloadingFileWatcher->get().AddFolder(*folderPath);
				}
			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateFromFiles(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = texture->m_filepaths;
		desc.IsCubemap = texture->m_textureType == TextureType::TextureCube;
		desc.Type = texture->m_textureCubeFormat;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromFile(std::string name,
																		                 std::filesystem::path filepath,
																		                 const TextureType type,
																		                 const TextureCubeFormat cubeFormat,
																		                 const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(!(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::NONE), "Texture::CreateFromFile(): Provided cube format is invalid");

	if(name.empty())
	{
		TP_WARN(Log::TexturePrefix, "Name is empty! Using filename as name!");
		name = filepath.string();
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::move(filepath), type, cubeFormat);

		//Hot Reloading
		const auto hotReloadingFileWatcher = TRAP::Application::GetHotReloadingFileWatcher();

		if(hotReloadingFileWatcher)
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					hotReloadingFileWatcher->get().AddFolder(*folderPath);
				}

			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateFromFile(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = texture->m_filepaths;
		desc.IsCubemap = texture->m_textureType == TextureType::TextureCube;
		desc.Type = texture->m_textureCubeFormat;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromFile(std::filesystem::path filepath,
																		                 const TextureType type,
		                                                                                 const TextureCubeFormat cubeFormat,
																		                 const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(!(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::NONE), "Texture::CreateFromFile(): Provided cube format is invalid");

	const auto name = FileSystem::GetFileNameWithoutEnding(filepath);
	if(!name)
	{
		TRAP_ASSERT(false, "Texture::CreateFromFile(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(*name, std::move(filepath), type, cubeFormat);

		//Hot Reloading
		const auto hotReloadingFileWatcher = TRAP::Application::GetHotReloadingFileWatcher();

		if(hotReloadingFileWatcher)
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					hotReloadingFileWatcher->get().AddFolder(*folderPath);
				}
			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateFromFile(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = texture->m_filepaths;
		desc.IsCubemap = texture->m_textureType == TextureType::TextureCube;
		desc.Type = texture->m_textureCubeFormat;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromImages(std::string name,
																			               const std::array<const Image*, 6>& imgs,
																			               const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(std::ranges::none_of(imgs, [](const Image* const img) { return img == nullptr; }),
	            "Texture::CreateFromImages(): An Image is nullptr!");

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::CreateFromImages(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	TRAP::Ref<Texture> texture = nullptr;

	std::array<std::filesystem::path, 6> filePaths{};
	for(usize i = 0; i < filePaths.size(); ++i)
		filePaths[i] = imgs[i]->GetFilePath();

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::move(filePaths));

		//Hot Reloading
		const auto hotReloadingFileWatcher = TRAP::Application::GetHotReloadingFileWatcher();

		if(hotReloadingFileWatcher)
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					hotReloadingFileWatcher->get().AddFolder(*folderPath);
				}
			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateFromImages(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Images = imgs;
		desc.IsCubemap = texture->m_textureType == TextureType::TextureCube;
		desc.Type = texture->m_textureCubeFormat;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromImage(std::string name,
																			              const TRAP::Image* const img,
 																	                      const TextureType type,
		                                                                                  const TextureCubeFormat cubeFormat,
																			              const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(img, "Texture::CreateFromImage(): Image is nullptr!");
	TRAP_ASSERT(cubeFormat != TextureCubeFormat::MultiFile, "Texture::CreateFromImage(): Provided cube format is invalid");
	TRAP_ASSERT(!(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::NONE), "Texture::CreateFromImage(): Provided cube format is invalid");

	if(name.empty() && !img->GetFilePath().empty())
	{
		TP_WARN(Log::TexturePrefix, "Name is empty! Using filename as name!");
		name = img->GetFilePath().filename().string();
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), img->GetFilePath(), type, cubeFormat);

		//Hot Reloading
		const auto hotReloadingFileWatcher = TRAP::Application::GetHotReloadingFileWatcher();

		if(hotReloadingFileWatcher)
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					hotReloadingFileWatcher->get().AddFolder(*folderPath);
				}
			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateFromImage(): Unknown RenderAPI");
		return nullptr;
	}

	if(texture)
	{
		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = texture->m_filepaths;
		desc.Images = {img};
		desc.IsCubemap = texture->m_textureType == TextureType::TextureCube;
		desc.Type = texture->m_textureCubeFormat;
		desc.CreationFlag = flags;
		desc.Texture = texture.get();

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateEmpty(std::string name,
																		              const u32 width,
																		              const u32 height,
																		              const u32 bitsPerPixel,
																		              const Image::ColorFormat format,
																		              const TextureType type,
																		              const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP::Ref<Texture> texture = nullptr;

	const API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(format, bitsPerPixel);

	if(imageFormat == API::ImageFormat::Undefined)
		return nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(type);
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Texture::CreateEmpty(): Unknown RenderAPI");
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
		texDesc.ArraySize = texture->m_textureType == TextureType::TextureCube ? 6 : 1;
		texDesc.Name = std::move(name);
		texDesc.Descriptors = texture->m_textureType == TextureType::TextureCube ? (RendererAPI::DescriptorType::Texture | RendererAPI::DescriptorType::TextureCube) :
																				   RendererAPI::DescriptorType::Texture;
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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    TRAP_ASSERT(desc.Width && desc.Height && (desc.Depth || desc.ArraySize), "Texture::CreateCustom(): Invalid TextureDesc!");

	if(!ValidateLimits(desc))
		return nullptr;

	TRAP::Ref<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(desc.ArraySize == 6 ? TextureType::TextureCube : TextureType::Texture2D);
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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const auto fallbackImg = TRAP::Image::LoadFallback();
	TRAP::Ref<TRAP::Graphics::Texture> fallback2DTex = CreateFromImage("Fallback2D",
																	   fallbackImg.get(),
																	   TextureType::Texture2D,
	                       											   TextureCubeFormat::NONE, TextureCreationFlags::Storage);

	fallback2DTex->AwaitLoading();

	//By default Storage texture are in Unordered Access layout.
	RendererAPI::Transition(fallback2DTex, RendererAPI::ResourceState::UnorderedAccess,
	                        RendererAPI::ResourceState::ShaderResource);

	return fallback2DTex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFallbackCube()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	std::array<TRAP::Scope<TRAP::Image>, 6> imgs{};
	std::array<const TRAP::Image*, 6> imgPtrs{};
	for(usize i = 0; i < imgs.size(); ++i)
	{
		imgs[i] = TRAP::Image::LoadFallback();
		imgPtrs[i] = imgs[i].get();
	}

	TRAP::Ref<TRAP::Graphics::Texture> fallbackCubeTex = CreateFromImages("FallbackCube", imgPtrs, TextureCreationFlags::Storage);
	fallbackCubeTex->AwaitLoading();

	//By default Storage texture are in Unordered Access layout.
	RendererAPI::Transition(fallbackCubeTex, RendererAPI::ResourceState::UnorderedAccess,
	                        RendererAPI::ResourceState::ShaderResource);

	return fallbackCubeTex;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Texture::Reload()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Can't reload if there is no filepath
	if(m_textureType == TextureType::Texture2D && std::get<0>(m_filepaths).empty())
		return false;
	if(m_textureType == TextureType::TextureCube)
	{
		for(const auto& path : m_filepaths)
		{
			if(path.empty())
				return false;
		}
	}

	//Make sure rendering using the texture finished before reloading it.
	RendererAPI::GetGraphicsQueue()->WaitQueueIdle();
	RendererAPI::GetComputeQueue()->WaitQueueIdle();

	//Shutdown the current texture
	Shutdown();

	//Load texture
	TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
	desc.Filepaths = m_filepaths;
	desc.IsCubemap = m_textureType == TextureType::TextureCube;
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

[[nodiscard]] const std::filesystem::path& TRAP::Graphics::Texture::GetFilePath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return std::get<0>(m_filepaths);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::Update(const void* const data, const u32 sizeInBytes, const u32 mipLevel,
                                     const u32 arrayLayer)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
		            static_cast<u8*>(updateDesc.MappedData));
	else //Needs row by row copy
	{
		for(usize r = 0; r < updateDesc.RowCount; ++r)
		{
			std::copy_n(static_cast<const u8*>(data) + r * updateDesc.SrcRowStride,
			            updateDesc.SrcRowStride,
						static_cast<u8*>(updateDesc.MappedData) + r * updateDesc.DstRowStride);
		}
	}
	TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Texture::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return true; //We don't have a valid sync token, so we assume the texture is loaded

	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return; //We don't have a valid sync token, so we assume the texture is loaded

	RendererAPI::GetResourceLoader()->WaitForToken(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::Texture::CalculateMipLevels(const u32 width, const u32 height)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Math::Max(1u, NumericCast<u32>(Math::Floor(Math::Log2(NumericCast<f32>(Math::Max(width, height))))) + 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Texture::ValidateLimits(const RendererAPI::TextureDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    if(desc.SampleCount > RendererAPI::SampleCount::One && desc.MipLevels > 1)
	{
		TP_ERROR(Log::TexturePrefix, "Multi-Sampled textures cannot have mip maps!");
		TRAP_ASSERT(false, "Texture::ValidateLimits(): Multi-Sampled textures cannot have mip maps!");
		return false;
	}
    const bool cubeMapRequired = (desc.Descriptors & RendererAPI::DescriptorType::TextureCube) ==
	                              RendererAPI::DescriptorType::TextureCube;
    if(!cubeMapRequired)
    {
        if(desc.Width > RendererAPI::GPUSettings.MaxImageDimension2D)
        {
            TP_ERROR(Log::TexturePrefix, "Texture width: ", desc.Width,
                     " is bigger than max allowed size: ", RendererAPI::GPUSettings.MaxImageDimension2D, "!");
            TRAP_ASSERT(false, "Texture::ValidateLimits(): Texture width is bigger than max allowed size!");
            return false;
        }
        if(desc.Height > RendererAPI::GPUSettings.MaxImageDimension2D)
        {
            TP_ERROR(Log::TexturePrefix, "Texture height: ", desc.Width,
                     " is bigger than max allowed size: ", RendererAPI::GPUSettings.MaxImageDimension2D, "!");
            TRAP_ASSERT(false, "Texture::ValidateLimits(): Texture height is bigger than max allowed size!");
            return false;
        }
    }
    else
    {
        if(desc.Width > RendererAPI::GPUSettings.MaxImageDimensionCube)
        {
            TP_ERROR(Log::TexturePrefix, "Texture width: ", desc.Width,
                     " is bigger than max allowed size: ", RendererAPI::GPUSettings.MaxImageDimensionCube, "!");
            TRAP_ASSERT(false, "Texture::ValidateLimits(): Texture width is bigger than max allowed size!");
            return false;
        }
        if(desc.Height > RendererAPI::GPUSettings.MaxImageDimensionCube)
        {
            TP_ERROR(Log::TexturePrefix, "Texture height: ", desc.Width,
                     " is bigger than max allowed size: ", RendererAPI::GPUSettings.MaxImageDimensionCube, "!");
            TRAP_ASSERT(false, "Texture::ValidateLimits(): Texture height is bigger than max allowed size!");
            return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ImageFormat TRAP::Graphics::Texture::ColorFormatBitsPerPixelToImageFormat(const Image::ColorFormat colorFormat,
	                             											                                 const u32 bpp)
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

TRAP::Graphics::Texture::Texture() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::Texture(std::string name, std::array<std::filesystem::path, 6> filepaths) noexcept
	: m_name(std::move(name)), m_filepaths(std::move(filepaths))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::Texture(std::string name, std::filesystem::path filepath, const TextureType type,
                                 const TextureCubeFormat cubeFormat) noexcept
	: m_name(std::move(name)), m_textureType(type), m_filepaths{std::move(filepath)}, m_textureCubeFormat(cubeFormat)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::Texture(const TextureType type) noexcept
	: m_textureType(type)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);
}