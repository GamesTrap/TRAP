#include "TRAPPCH.h"
#include "Texture.h"

#include "Application.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"
#include "Graphics/API/Objects/Queue.h"

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromFiles(std::string name,
																			std::array<std::filesystem::path, 6> filepaths,
																			const TextureCreationFlags flags)
{
	ZoneScoped;

	if(name.empty())
	{
		TRAP_ASSERT(false, "Name is empty!");
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
				const auto folderPath = FileSystem::GetFolderPath(path);
				if(!folderPath)
					continue;

				TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromFile(std::string name,
																		   std::filesystem::path filepath,
																		   const TextureType type,
																		   const TextureCubeFormat cubeFormat,
																		   const TextureCreationFlags flags)
{
	ZoneScoped;

	TRAP_ASSERT(!(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::NONE), "Provided cube format is invalid");

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
				const auto folderPath = FileSystem::GetFolderPath(path);
				if(!folderPath)
					continue;

				TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromFile(std::filesystem::path filepath,
																		   const TextureType type,
		                                                                   const TextureCubeFormat cubeFormat,
																		   const TextureCreationFlags flags)
{
	ZoneScoped;

	TRAP_ASSERT(!(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::NONE), "Provided cube format is invalid");

	const auto name = FileSystem::GetFileName(filepath);
	if(!name)
	{
		TRAP_ASSERT(false, "Name is empty!");
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
				const auto folderPath = FileSystem::GetFolderPath(path);
				if(!folderPath)
					continue;

				TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromImages(std::string name,
																			 const std::array<const Image*, 6>& imgs,
																			 const TextureCreationFlags flags)
{
	ZoneScoped;

	TRAP_ASSERT(std::none_of(imgs.cbegin(), imgs.cend(),
	            [](const Image* const img) { return img == nullptr; }), "An Image is nullptr!");

	if(name.empty())
	{
		TRAP_ASSERT(false, "Name is empty!");
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
				const auto folderPath = FileSystem::GetFolderPath(path);
				if(!folderPath)
					continue;

				TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromImage(std::string name,
																			const TRAP::Image* const img,
 																	        const TextureType type,
		                                                                    const TextureCubeFormat cubeFormat,
																			const TextureCreationFlags flags)
{
	ZoneScoped;

	TRAP_ASSERT(img, "Image is nullptr!");
	TRAP_ASSERT(cubeFormat != TextureCubeFormat::MultiFile, "Provided cube format is invalid");
	TRAP_ASSERT(!(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::NONE), "Provided cube format is invalid");

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
				const auto folderPath = FileSystem::GetFolderPath(path);
				if(!folderPath)
					continue;

				TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
			}
		}

		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateEmpty(std::string name,
																		const uint32_t width,
																		const uint32_t height,
																		const uint32_t bitsPerPixel,
																		const Image::ColorFormat format,
																		const TextureType type,
																		const TextureCreationFlags flags)
{
	ZoneScoped;

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
		TRAP_ASSERT(false, "Unknown RenderAPI");
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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateCustom(const TRAP::Graphics::RendererAPI::TextureDesc& desc)
{
	ZoneScoped;

    TRAP_ASSERT(desc.Width && desc.Height && (desc.Depth || desc.ArraySize));

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
		TRAP_ASSERT(false, "Unknown RenderAPI");
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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFallback2D()
{
	ZoneScoped;

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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFallbackCube()
{
	ZoneScoped;

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
	ZoneScoped;

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

std::string TRAP::Graphics::Texture::GetName() const
{
	ZoneScoped;

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureType TRAP::Graphics::Texture::GetType() const
{
	ZoneScoped;

	return m_textureType;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetWidth() const
{
	ZoneScoped;

	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetHeight() const
{
	ZoneScoped;

	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2ui TRAP::Graphics::Texture::GetSize() const
{
	ZoneScoped;

	return Math::Vec2ui(m_width, m_height);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetDepth() const
{
	ZoneScoped;

	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetArraySize() const
{
	ZoneScoped;

	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipLevels() const
{
	ZoneScoped;

	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetAspectMask() const
{
	ZoneScoped;

	return m_aspectMask;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::ColorFormat TRAP::Graphics::Texture::GetColorFormat() const
{
	ZoneScoped;

	return ImageFormatToColorFormat(m_imageFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::Texture::GetImageFormat() const
{
	ZoneScoped;

	return m_imageFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::Texture::GetDescriptorTypes() const
{
	ZoneScoped;

	return m_descriptorTypes;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBitsPerChannel() const
{
	ZoneScoped;

	return GetBitsPerChannelFromImageFormat(m_imageFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBytesPerChannel() const
{
	ZoneScoped;

	return GetBitsPerChannel() / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBitsPerPixel() const
{
	ZoneScoped;

	return GetBitsPerChannel() * static_cast<uint32_t>(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBytesPerPixel() const
{
	ZoneScoped;

	return GetBitsPerPixel() / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipWidth(const uint32_t mipLevel) const
{
	ZoneScoped;

	if(mipLevel >= m_mipLevels)
		TP_ERROR(Log::TexturePrefix, "GetMipWidth: Invalid mip level provided!");

	return Math::Max(1u, m_width >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipHeight(const uint32_t mipLevel) const
{
	ZoneScoped;

	if(mipLevel >= m_mipLevels)
		TP_ERROR(Log::TexturePrefix, "GetMipHeight: Invalid mip level provided!");

	return Math::Max(1u, m_height >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2ui TRAP::Graphics::Texture::GetMipSize(const uint32_t mipLevel) const
{
	ZoneScoped;

	return TRAP::Math::Vec2ui{GetMipWidth(mipLevel), GetMipHeight(mipLevel)};
}

//-------------------------------------------------------------------------------------------------------------------//

const std::filesystem::path& TRAP::Graphics::Texture::GetFilePath() const
{
	ZoneScoped;

	return m_filepaths[0];
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<std::filesystem::path, 6>& TRAP::Graphics::Texture::GetFilePaths() const
{
	ZoneScoped;

	return m_filepaths;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCubeFormat TRAP::Graphics::Texture::GetCubeFormat() const
{
	ZoneScoped;

	return m_textureCubeFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::Update(const void* const data, const uint32_t sizeInBytes, const uint32_t mipLevel,
                                     const uint32_t arrayLayer)
{
	ZoneScoped;

	TRAP_ASSERT(data, "Update: Data is nullptr!");
	TRAP_ASSERT(arrayLayer < m_arraySize, "Invalid array layer provided!");
	TRAP_ASSERT(mipLevel < m_mipLevels, "Invalid mip level provided!");
	TRAP_ASSERT(sizeInBytes >= (m_width >> mipLevel) * (m_height >> mipLevel) * GetBytesPerPixel(),
	            "Texture update size is too small");

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

bool TRAP::Graphics::Texture::OwnsImage() const
{
	ZoneScoped;

	return m_ownsImage;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Texture::IsLoaded() const
{
	ZoneScoped;

	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return true; //We don't have a valid sync token, so we assume the texture is loaded

	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::AwaitLoading() const
{
	ZoneScoped;

	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return; //We don't have a valid sync token, so we assume the texture is loaded

	RendererAPI::GetResourceLoader()->WaitForToken(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::CalculateMipLevels(const uint32_t width, const uint32_t height)
{
	ZoneScoped;

	return Math::Max(1u, static_cast<uint32_t>(Math::Floor(Math::Log2(Math::Max(static_cast<float>(width),
					                                                            static_cast<float>(height))))) + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Texture::ValidateLimits(const RendererAPI::TextureDesc& desc)
{
	ZoneScoped;

    if(desc.SampleCount > RendererAPI::SampleCount::One && desc.MipLevels > 1)
	{
		TP_ERROR(Log::TexturePrefix, "Multi-Sampled textures cannot have mip maps!");
		TRAP_ASSERT(false);
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
            TRAP_ASSERT(false);
            return false;
        }
        if(desc.Height > RendererAPI::GPUSettings.MaxImageDimension2D)
        {
            TP_ERROR(Log::TexturePrefix, "Texture height: ", desc.Width,
                     " is bigger than max allowed size: ", RendererAPI::GPUSettings.MaxImageDimension2D, "!");
            TRAP_ASSERT(false);
            return false;
        }
    }
    else
    {
        if(desc.Width > RendererAPI::GPUSettings.MaxImageDimensionCube)
        {
            TP_ERROR(Log::TexturePrefix, "Texture width: ", desc.Width,
                     " is bigger than max allowed size: ", RendererAPI::GPUSettings.MaxImageDimensionCube, "!");
            TRAP_ASSERT(false);
            return false;
        }
        if(desc.Height > RendererAPI::GPUSettings.MaxImageDimensionCube)
        {
            TP_ERROR(Log::TexturePrefix, "Texture height: ", desc.Width,
                     " is bigger than max allowed size: ", RendererAPI::GPUSettings.MaxImageDimensionCube, "!");
            TRAP_ASSERT(false);
            return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Graphics::Texture::Rotate90Clockwise(const TRAP::Image* const img)
{
	ZoneScoped;

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

TRAP::Scope<TRAP::Image> TRAP::Graphics::Texture::Rotate90CounterClockwise(const TRAP::Image* const img)
{
	ZoneScoped;

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

TRAP::Graphics::Texture::Texture()
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
	ZoneScoped;
}
