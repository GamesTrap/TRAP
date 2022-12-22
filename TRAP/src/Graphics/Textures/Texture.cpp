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
		if(TRAP::Application::IsHotReloadingEnabled())
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
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
		if(TRAP::Application::IsHotReloadingEnabled())
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
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
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(*name), std::move(filepath), type, cubeFormat);

		//Hot Reloading
		if(TRAP::Application::IsHotReloadingEnabled())
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
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

	TRAP_ASSERT(std::none_of(imgs.cbegin(), imgs.cend(),
	            [](const Image* const img) { return img == nullptr; }), "Texture::CreateFromImages(): An Image is nullptr!");

	if(name.empty())
	{
		TRAP_ASSERT(false, "Texture::CreateFromImages(): Name is empty!");
		TP_ERROR(Log::TexturePrefix, "Name is empty!");
		return nullptr;
	}

	TRAP::Ref<Texture> texture = nullptr;

	std::array<std::filesystem::path, 6> filePaths{};
	for(std::size_t i = 0; i < filePaths.size(); ++i)
		filePaths[i] = imgs[i]->GetFilePath();

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), std::move(filePaths));

		//Hot Reloading
		if(TRAP::Application::IsHotReloadingEnabled())
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
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
		name = img->GetFilePath().filename().u8string();
	}

	TRAP::Ref<Texture> texture = nullptr;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeRef<API::VulkanTexture>(std::move(name), img->GetFilePath(), type, cubeFormat);

		//Hot Reloading
		if(TRAP::Application::IsHotReloadingEnabled())
		{
			for(const std::filesystem::path& path : texture->m_filepaths)
			{
				if(!path.empty())
				{
					const auto folderPath = FileSystem::GetFolderPath(path);
					if(!folderPath)
						continue;

					TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
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
																		              const uint32_t width,
																		              const uint32_t height,
																		              const uint32_t bitsPerPixel,
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
		if(static_cast<bool>(flags & TextureCreationFlags::Storage))
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
	for(std::size_t i = 0; i < imgs.size(); ++i)
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
	if(m_textureType == TextureType::Texture2D && m_filepaths[0].empty())
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

	if(static_cast<bool>(m_descriptorTypes & RendererAPI::DescriptorType::RWTexture))
		desc.CreationFlag |= RendererAPI::TextureCreationFlags::Storage;

	TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &m_syncToken);

	AwaitLoading();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Graphics::Texture::GetName() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::TextureType TRAP::Graphics::Texture::GetType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_textureType;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetWidth() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetHeight() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2ui TRAP::Graphics::Texture::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Math::Vec2ui(m_width, m_height);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetDepth() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetArraySize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetMipLevels() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetAspectMask() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_aspectMask;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Image::ColorFormat TRAP::Graphics::Texture::GetColorFormat() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return ImageFormatToColorFormat(m_imageFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ImageFormat TRAP::Graphics::Texture::GetImageFormat() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_imageFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::Texture::GetDescriptorTypes() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_descriptorTypes;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetBitsPerChannel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetBitsPerChannelFromImageFormat(m_imageFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetBytesPerChannel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetBitsPerChannel() / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetBitsPerPixel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetBitsPerChannel() * static_cast<uint32_t>(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetBytesPerPixel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetBitsPerPixel() / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetMipWidth(const uint32_t mipLevel) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(mipLevel >= m_mipLevels)
		TP_ERROR(Log::TexturePrefix, "GetMipWidth: Invalid mip level provided!");

	return Math::Max(1u, m_width >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::Texture::GetMipHeight(const uint32_t mipLevel) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(mipLevel >= m_mipLevels)
		TP_ERROR(Log::TexturePrefix, "GetMipHeight: Invalid mip level provided!");

	return Math::Max(1u, m_height >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2ui TRAP::Graphics::Texture::GetMipSize(const uint32_t mipLevel) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return TRAP::Math::Vec2ui{GetMipWidth(mipLevel), GetMipHeight(mipLevel)};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::filesystem::path& TRAP::Graphics::Texture::GetFilePath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_filepaths[0];
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::array<std::filesystem::path, 6>& TRAP::Graphics::Texture::GetFilePaths() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_filepaths;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::TextureCubeFormat TRAP::Graphics::Texture::GetCubeFormat() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_textureCubeFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::Update(const void* const data, const uint32_t sizeInBytes, const uint32_t mipLevel,
                                     const uint32_t arrayLayer)
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
	TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(updateDesc);
	if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single copy is enough
		std::copy_n(static_cast<const uint8_t*>(data), updateDesc.RowCount * updateDesc.SrcRowStride,
		            static_cast<uint8_t*>(updateDesc.MappedData));
	else //Needs row by row copy
	{
		for(std::size_t r = 0; r < updateDesc.RowCount; ++r)
		{
			std::copy_n(static_cast<const uint8_t*>(data) + r * updateDesc.SrcRowStride,
			            updateDesc.SrcRowStride,
						static_cast<uint8_t*>(updateDesc.MappedData) + r * updateDesc.DstRowStride);
		}
	}
	TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Texture::OwnsImage() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_ownsImage;
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

[[nodiscard]] uint32_t TRAP::Graphics::Texture::CalculateMipLevels(const uint32_t width, const uint32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Math::Max(1u, static_cast<uint32_t>(Math::Floor(Math::Log2(Math::Max(static_cast<float>(width),
					                                                            static_cast<float>(height))))) + 1);
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

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Graphics::Texture::Rotate90Clockwise(const TRAP::Image* const img)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(img->GetBitsPerChannel() == 32)
	{
		std::vector<float> rotated(static_cast<std::size_t>(img->GetWidth()) * img->GetHeight() *
														    static_cast<std::size_t>(img->GetColorFormat()));
		for(uint32_t y = 0, destCol = img->GetHeight() - 1; y < img->GetHeight(); ++y, --destCol)
		{
			const uint32_t offset = y * img->GetWidth();

			for(uint32_t x = 0; x < img->GetWidth(); ++x)
			{
				for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
				{
					rotated[(x * img->GetHeight() + destCol) * static_cast<uint32_t>(img->GetColorFormat()) + channel] =
					static_cast<const float*>(img->GetPixelData())[(offset + x) * static_cast<uint32_t>(img->GetColorFormat()) + channel];
				}
			}
		}
		return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}
	if(img->GetBitsPerChannel() == 16)
	{
		std::vector<uint16_t> rotated(static_cast<std::size_t>(img->GetWidth()) * img->GetHeight() *
															   static_cast<std::size_t>(img->GetColorFormat()));
		for(uint32_t y = 0, destCol = img->GetHeight() - 1; y < img->GetHeight(); ++y, --destCol)
		{
			const uint32_t offset = y * img->GetWidth();

			for(uint32_t x = 0; x < img->GetWidth(); ++x)
			{
				for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
				{
					rotated[(x * img->GetHeight() + destCol) * static_cast<uint32_t>(img->GetColorFormat()) + channel] =
					static_cast<const uint16_t*>(img->GetPixelData())[(offset + x) * static_cast<uint32_t>(img->GetColorFormat()) + channel];
				}
			}
		}
		return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}

	std::vector<uint8_t> rotated(static_cast<std::size_t>(img->GetWidth()) * img->GetHeight() *
								 static_cast<std::size_t>(img->GetColorFormat()));
	for(uint32_t y = 0, destCol = img->GetHeight() - 1; y < img->GetHeight(); ++y, --destCol)
	{
		const uint32_t offset = y * img->GetWidth();

		for(uint32_t x = 0; x < img->GetWidth(); ++x)
		{
			for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
			{
				rotated[(x * img->GetHeight() + destCol) * static_cast<uint32_t>(img->GetColorFormat()) + channel] =
				static_cast<const uint8_t*>(img->GetPixelData())[(offset + x) * static_cast<uint32_t>(img->GetColorFormat()) + channel];
			}
		}
	}

	return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Graphics::Texture::Rotate90CounterClockwise(const TRAP::Image* const img)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(img->GetBitsPerChannel() == 32)
	{
		std::vector<float> rotated(static_cast<std::size_t>(img->GetWidth()) * img->GetHeight() *
								   static_cast<std::size_t>(img->GetColorFormat()));
		std::copy_n(static_cast<const uint8_t*>(img->GetPixelData()), img->GetPixelDataSize(), rotated.begin());
		for(uint32_t x = 0; x < img->GetWidth(); ++x)
		{
			for(uint32_t y = 0; y < img->GetHeight(); ++y)
			{
				uint32_t I = y;
				uint32_t J = img->GetWidth() - 1 - x;
				while((x * img->GetHeight() + y) > (I * img->GetWidth() + J))
				{
					const uint32_t p = I * img->GetWidth() + J;
					I = p % img->GetHeight();
					J = img->GetWidth() - 1 - (p / img->GetHeight());
				}
				for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
				{
					std::swap(rotated[(x * img->GetHeight() + y) * static_cast<uint32_t>(img->GetColorFormat()) + channel],
							  rotated[(I * img->GetWidth() + J) * static_cast<uint32_t>(img->GetColorFormat()) + channel]);
				}
			}
		}

		return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}
	if(img->GetBitsPerChannel() == 16)
	{
		std::vector<uint16_t> rotated(static_cast<std::size_t>(img->GetWidth()) * img->GetHeight() * static_cast<std::size_t>(img->GetColorFormat()));
		std::copy_n(static_cast<const uint8_t*>(img->GetPixelData()), img->GetPixelDataSize(), rotated.begin());
		for(uint32_t x = 0; x < img->GetWidth(); ++x)
		{
			for(uint32_t y = 0; y < img->GetHeight(); ++y)
			{
				uint32_t I = y;
				uint32_t J = img->GetWidth() - 1 - x;
				while((x * img->GetHeight() + y) > (I * img->GetWidth() + J))
				{
					const uint32_t p = I * img->GetWidth() + J;
					I = p % img->GetHeight();
					J = img->GetWidth() - 1 - (p / img->GetHeight());
				}
				for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
				{
					std::swap(rotated[(x * img->GetHeight() + y) * static_cast<uint32_t>(img->GetColorFormat()) + channel],
							  rotated[(I * img->GetWidth() + J) * static_cast<uint32_t>(img->GetColorFormat()) + channel]);
				}
			}
		}

		return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
	}

	std::vector<uint8_t> rotated(static_cast<std::size_t>(img->GetWidth()) * img->GetHeight() *
								 static_cast<std::size_t>(img->GetColorFormat()));
	std::copy_n(static_cast<const uint8_t*>(img->GetPixelData()), img->GetPixelDataSize(), rotated.begin());
	for(uint32_t x = 0; x < img->GetWidth(); ++x)
	{
		for(uint32_t y = 0; y < img->GetHeight(); ++y)
		{
			uint32_t I = y;
			uint32_t J = img->GetWidth() - 1 - x;
			while((x * img->GetHeight() + y) > (I * img->GetWidth() + J))
			{
				const uint32_t p = I * img->GetWidth() + J;
				I = p % img->GetHeight();
				J = img->GetWidth() - 1 - (p / img->GetHeight());
			}
			for(uint32_t channel = 0; channel < static_cast<uint32_t>(img->GetColorFormat()); ++channel)
			{
				std::swap(rotated[(x * img->GetHeight() + y) * static_cast<uint32_t>(img->GetColorFormat()) + channel],
						  rotated[(I * img->GetWidth() + J) * static_cast<uint32_t>(img->GetColorFormat()) + channel]);
			}
		}
	}

	return TRAP::Image::LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), rotated);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::Texture() noexcept
	: m_name(),
	  m_syncToken(0),
	  m_textureType(TextureType::Texture2D),
	  m_width(2),
	  m_height(2),
	  m_depth(1),
	  m_arraySize(1),
	  m_mipLevels(1),
	  m_colorFormat(Image::ColorFormat::RGBA),
	  m_imageFormat(API::ImageFormat::R8G8B8A8_UNORM),
	  m_aspectMask(0),
	  m_descriptorTypes(RendererAPI::DescriptorType::Texture),
	  m_ownsImage(true),
	  m_textureCubeFormat(TextureCubeFormat::NONE)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);
}
