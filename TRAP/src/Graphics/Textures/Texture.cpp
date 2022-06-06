#include "TRAPPCH.h"
#include "Texture.h"

#include "Application.h"
#include "FS/FS.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromFiles(std::string name,
																			  std::array<std::filesystem::path, 6> filepaths,
																			  const TextureCreationFlags flags)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_ERROR(Log::TexturePrefix, "Invalid name!");
		return nullptr;
	}

	TRAP::Scope<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeScope<API::VulkanTexture>(std::move(name), std::move(filepaths));
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

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromFile(std::string name,
																			 std::filesystem::path filepath,
																			 const TextureType type,
																			 const TextureCubeFormat cubeFormat,
																			 const TextureCreationFlags flags)
{
	TRAP_ASSERT(!(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::NONE), "Provided cube format is invalid");

	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::TexturePrefix, "Name is empty! Using filename as name!");
		name = filepath;
	}

	TRAP::Scope<Texture> texture = nullptr;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeScope<API::VulkanTexture>(std::move(name), std::move(filepath), type, cubeFormat);
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

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromFile(std::filesystem::path filepath,
																			 const TextureType type,
		                                                                     const TextureCubeFormat cubeFormat,
																			 const TextureCreationFlags flags)
{
	TRAP_ASSERT(!(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::NONE), "Provided cube format is invalid");

	TP_PROFILE_FUNCTION();

	const std::string name = FS::GetFileName(filepath);

	TRAP::Scope<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeScope<API::VulkanTexture>(std::move(name), std::move(filepath), type, cubeFormat);
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

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromImages(std::string name,
																			   const std::array<Image*, 6>& imgs,
																			   const TextureCreationFlags flags)
{
	TRAP_ASSERT(std::none_of(imgs.cbegin(), imgs.cend(),
	            [](const Image* img) { return img == nullptr; }), "An Image is nullptr!");

	TP_PROFILE_FUNCTION();

	//Validation that images have same size and format
	Image::ColorFormat format = imgs[0]->GetColorFormat();
	if(std::none_of(imgs.cbegin(), imgs.cend(),
	   [&](const Image* img) {return img->GetColorFormat() != format ||
	                                 img->GetBitsPerChannel() != imgs[0]->GetBitsPerPixel() ||
									 img->GetWidth() != imgs[0]->GetWidth() ||
									 img->GetHeight() != imgs[0]->GetHeight(); }))
	{
		TP_ERROR(Log::TexturePrefix, "An image has mismatching color format, bits per channel, width and/or height!");
		return nullptr;
	}

	//Convert to RGBA if necessary
	std::array<Scope<Image>, 6> imgsRGBA{};
	std::array<Image*, 6> imgsRGBAPtr{};
	if(format == Image::ColorFormat::RGB)
	{
		for(uint32_t i = 0; i < imgs.size(); ++i)
		{
			imgsRGBA[i] = TRAP::Image::ConvertRGBToRGBA(imgs[i]);
			imgsRGBAPtr[i] = imgsRGBA[i].get();
		}
	}

	const std::array<Image*, 6>& useImgs = (format == Image::ColorFormat::RGB ? imgsRGBAPtr : imgs);

	TRAP::Scope<Texture> texture = nullptr;

	const API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(useImgs[0]->GetColorFormat(),
																			  useImgs[0]->GetBitsPerPixel());

	if(imageFormat == API::ImageFormat::Undefined)
		return nullptr;

	std::array<std::filesystem::path, 6> filepaths{};
	for(uint32_t i = 0; i < useImgs.size(); ++i)
		filepaths[i] = useImgs[i]->GetFilePath();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeScope<API::VulkanTexture>(std::move(name), filepaths);
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
		//Create empty Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc loadDesc{};
		loadDesc.Texture = texture.get();

		RendererAPI::TextureDesc texDesc{};
		texDesc.Width = useImgs[0]->GetWidth();
		texDesc.Height = useImgs[0]->GetHeight();
		texDesc.ArraySize = 6;
		texDesc.MipLevels = CalculateMipLevels(useImgs[0]->GetWidth(), useImgs[0]->GetHeight());
		texDesc.Format = imageFormat;
		texDesc.StartState = RendererAPI::ResourceState::Common;
		texDesc.Descriptors = RendererAPI::DescriptorType::Texture | RendererAPI::DescriptorType::TextureCube;
		if(static_cast<bool>(flags & TextureCreationFlags::Storage))
			texDesc.Descriptors |= RendererAPI::DescriptorType::RWTexture;

		loadDesc.Desc = &texDesc;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(loadDesc, &texture->m_syncToken);

		//Wait for texture to be ready
		RendererAPI::GetResourceLoader()->WaitForToken(&texture->m_syncToken);

		//Fill empty Texture with images pixel data
		for(uint32_t i = 0; i < useImgs.size(); ++i)
			texture->Update(useImgs[i]->GetPixelData(), useImgs[i]->GetPixelDataSize(), 0, i);

		//TODO Mipmaps
		//TODO Make use of the ResourceLoader to do all this stuff
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFromImage(std::string name,
																			  const TRAP::Image* const img,
 																	          const TextureType type,
		                                                                      const TextureCubeFormat cubeFormat,
																			  const TextureCreationFlags flags)
{
	TRAP_ASSERT(img, "Image is nullptr!");
	TRAP_ASSERT(!(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::NONE), "Provided cube format is invalid");
	TRAP_ASSERT(cubeFormat != TextureCubeFormat::MultiFile, "Provided cube format is invalid");

	TP_PROFILE_FUNCTION();

	TRAP::Scope<Image> imgRGBA = nullptr;
	if(img->GetColorFormat() == Image::ColorFormat::RGB)
		imgRGBA = TRAP::Image::ConvertRGBToRGBA(img);

	const Image* const useImg = imgRGBA ? imgRGBA.get() : img;

	TRAP::Scope<Texture> texture = nullptr;

	const API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(useImg->GetColorFormat(), useImg->GetBitsPerPixel());

	if(imageFormat == API::ImageFormat::Undefined)
		return nullptr;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeScope<API::VulkanTexture>(std::move(name), img->GetFilePath(), type, cubeFormat);
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
		//Create empty Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc loadDesc{};
		loadDesc.Texture = texture.get();

		RendererAPI::TextureDesc texDesc{};
		texDesc.Format = imageFormat;
		texDesc.StartState = RendererAPI::ResourceState::Common;
		texDesc.Descriptors = texture->m_textureType == TextureType::TextureCube ? (RendererAPI::DescriptorType::Texture | RendererAPI::DescriptorType::TextureCube) :
																				   RendererAPI::DescriptorType::Texture;
		if(static_cast<bool>(flags & TextureCreationFlags::Storage))
			texDesc.Descriptors |= RendererAPI::DescriptorType::RWTexture;

		std::array<TRAP::Scope<Image>, 6> faces{};
		if(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::Cross)
		{
			if(useImg->GetBytesPerChannel() == 1)
				faces = SplitImageFromCross<uint8_t>(imgRGBA ? imgRGBA.get() : img);
			else if(useImg->GetBytesPerChannel() == 2)
				faces = SplitImageFromCross<uint16_t>(imgRGBA ? imgRGBA.get() : img);
			else if(useImg->GetBytesPerChannel() == 4)
				faces = SplitImageFromCross<float>(imgRGBA ? imgRGBA.get() : img);

			texDesc.MipLevels = CalculateMipLevels(faces[0]->GetWidth(), faces[0]->GetHeight());
			texDesc.Width = faces[0]->GetWidth();
			texDesc.Height = faces[0]->GetWidth();
			texDesc.ArraySize = 6;
		}
		else if(type == TextureType::Texture2D)
		{
			texDesc.Width = useImg->GetWidth();
			texDesc.Height = useImg->GetHeight();
			texDesc.MipLevels = CalculateMipLevels(useImg->GetWidth(), useImg->GetHeight());
		}

		loadDesc.Desc = &texDesc;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(loadDesc, &texture->m_syncToken);

		//Wait for texture to be ready
		RendererAPI::GetResourceLoader()->WaitForToken(&texture->m_syncToken);

		//Upload image data
		if(type == TextureType::TextureCube && cubeFormat == TextureCubeFormat::Cross)
		{
			for(uint32_t i = 0; i < faces.size(); ++i)
				texture->Update(faces[i]->GetPixelData(), faces[i]->GetPixelDataSize(), 0, i);
		}
		else if(type == TextureType::Texture2D)
			texture->Update(useImg->GetPixelData(), useImg->GetPixelDataSize());

		//TODO Mipmaps
		//TODO Make use of the ResourceLoader to do all this stuff
	}

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateEmpty(std::string name,
																		  const uint32_t width,
																		  const uint32_t height,
																		  const uint32_t bitsPerPixel,
																		  const Image::ColorFormat format,
																		  const TextureType type,
																		  const TextureCreationFlags flags)
{
	TP_PROFILE_FUNCTION();

	TRAP::Scope<Texture> texture = nullptr;

	const API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(format, bitsPerPixel);

	if(imageFormat == API::ImageFormat::Undefined)
		return nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeScope<API::VulkanTexture>(type);
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

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateCustom(const TRAP::Graphics::RendererAPI::TextureDesc& desc)
{
    TRAP_ASSERT(desc.Width && desc.Height && (desc.Depth || desc.ArraySize));

	TP_PROFILE_FUNCTION();

	if(!ValidateLimits(desc))
		return nullptr;

	TRAP::Scope<Texture> texture = nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		texture = TRAP::MakeScope<API::VulkanTexture>(desc.ArraySize == 6 ? TextureType::TextureCube : TextureType::Texture2D);
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

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFallback2D()
{
	TRAP::Scope<TRAP::Graphics::Texture> fallback2DTex = CreateFromImage("Fallback2D",
																		 TRAP::Image::LoadFallback().get(),
																		 TextureType::Texture2D,
	                       												 TextureCubeFormat::NONE, TextureCreationFlags::Storage);

	fallback2DTex->AwaitLoading();

	//By default Storage texture are in Unordered Access layout.
	RendererAPI::Transition(fallback2DTex.get(), RendererAPI::ResourceState::UnorderedAccess,
	                        RendererAPI::ResourceState::ShaderResource);

	return fallback2DTex;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::Texture::CreateFallbackCube()
{
	std::array<TRAP::Scope<TRAP::Image>, 6> imgs{};
	std::array<TRAP::Image*, 6> imgPtrs{};
	for(uint32_t i = 0; i < imgs.size(); ++i)
	{
		imgs[i] = TRAP::Image::LoadFallback();
		imgPtrs[i] = imgs[i].get();
	}

	TRAP::Scope<TRAP::Graphics::Texture> fallbackCubeTex = CreateFromImages("FallbackCube", imgPtrs, TextureCreationFlags::Storage);
	fallbackCubeTex->AwaitLoading();

	//By default Storage texture are in Unordered Access layout.
	RendererAPI::Transition(fallbackCubeTex.get(), RendererAPI::ResourceState::UnorderedAccess,
	                        RendererAPI::ResourceState::ShaderResource);

	return fallbackCubeTex;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Texture::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureType TRAP::Graphics::Texture::GetType() const
{
	return m_textureType;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2ui TRAP::Graphics::Texture::GetSize() const
{
	return Math::Vec2ui(m_width, m_height);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetDepth() const
{
	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetArraySize() const
{
	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipLevels() const
{
	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetAspectMask() const
{
	return m_aspectMask;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::ColorFormat TRAP::Graphics::Texture::GetColorFormat() const
{
	return ImageFormatToColorFormat(m_imageFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::Texture::GetImageFormat() const
{
	return m_imageFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::Texture::GetDescriptorTypes() const
{
	return m_descriptorTypes;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBitsPerChannel() const
{
	return GetBitsPerChannelFromImageFormat(m_imageFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBytesPerChannel() const
{
	return GetBitsPerChannel() / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBitsPerPixel() const
{
	return GetBitsPerChannel() * static_cast<uint32_t>(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBytesPerPixel() const
{
	return GetBitsPerPixel() / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipWidth(const uint32_t mipLevel) const
{
	if(mipLevel >= m_mipLevels)
		TP_ERROR(Log::TexturePrefix, "GetMipWidth: Invalid mip level provided!");

	return Math::Max(1u, m_width >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipHeight(const uint32_t mipLevel) const
{
	if(mipLevel >= m_mipLevels)
		TP_ERROR(Log::TexturePrefix, "GetMipHeight: Invalid mip level provided!");

	return Math::Max(1u, m_height >> mipLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2ui TRAP::Graphics::Texture::GetMipSize(const uint32_t mipLevel) const
{
	return TRAP::Math::Vec2ui{GetMipWidth(mipLevel), GetMipHeight(mipLevel)};
}

//-------------------------------------------------------------------------------------------------------------------//

const std::filesystem::path& TRAP::Graphics::Texture::GetFilePath() const
{
	return m_filepaths[0];
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<std::filesystem::path, 6>& TRAP::Graphics::Texture::GetFilePaths() const
{
	return m_filepaths;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCubeFormat TRAP::Graphics::Texture::GetCubeFormat() const
{
	return m_textureCubeFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::Update(const void* data, const uint32_t sizeInBytes, const uint32_t mipLevel,
                                     const uint32_t arrayLayer)
{
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
	if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single memcpy is enough
		memcpy(updateDesc.MappedData, data, updateDesc.RowCount * updateDesc.SrcRowStride);
	else //Needs row by row copy
	{
		for(std::size_t r = 0; r < updateDesc.RowCount; ++r)
		{
			memcpy(updateDesc.MappedData + r * updateDesc.DstRowStride,
				   static_cast<const uint8_t*>(data) + r * updateDesc.SrcRowStride,
				   updateDesc.SrcRowStride);
		}
	}
	TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Texture::OwnsImage() const
{
	return m_ownsImage;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Texture::IsLoaded() const
{
	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return true; //We don't have a valid sync token, so we assume the texture is loaded

	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::AwaitLoading() const
{
	if(m_syncToken == std::numeric_limits<API::SyncToken>::max())
		return; //We don't have a valid sync token, so we assume the texture is loaded

	RendererAPI::GetResourceLoader()->WaitForToken(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::CalculateMipLevels(const uint32_t width, const uint32_t height)
{
	return Math::Max(1u, static_cast<uint32_t>(Math::Floor(Math::Log2(Math::Max(static_cast<float>(width),
					                                                            static_cast<float>(height))))) + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Texture::ValidateLimits(const RendererAPI::TextureDesc& desc)
{
    if(desc.SampleCount > RendererAPI::SampleCount::SampleCount1 && desc.MipLevels > 1)
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

TRAP::Graphics::API::ImageFormat TRAP::Graphics::Texture::ColorFormatBitsPerPixelToImageFormat(const Image::ColorFormat colorFormat,
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

		TRAP_ASSERT(false, "Invalid bits per pixel & color format combination provided!");
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

		TRAP_ASSERT(false, "Invalid bits per pixel & color format combination provided!");
		return API::ImageFormat::Undefined;
	}
	if(colorFormat == Image::ColorFormat::RGB)
	{
		TRAP_ASSERT(false, "Color format RGB is not allowed on empty textures as GPU needs an alpha channel!");
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

		TRAP_ASSERT(false, "Invalid bits per pixel & color format combination provided!");
		return API::ImageFormat::Undefined;
	}

	TRAP_ASSERT(false, "Invalid color format provided!");
	return API::ImageFormat::Undefined;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image::ColorFormat TRAP::Graphics::Texture::ImageFormatToColorFormat(const API::ImageFormat imageFormat)
{
	switch(imageFormat)
	{
	case API::ImageFormat::R8_UNORM:
	case API::ImageFormat::R16_UNORM:
	case API::ImageFormat::R32_SFLOAT:
		return Image::ColorFormat::GrayScale;

	case API::ImageFormat::R8G8_UNORM:
	case API::ImageFormat::R16G16_UNORM:
	case API::ImageFormat::R32G32_SFLOAT:
		return Image::ColorFormat::GrayScaleAlpha;

	case API::ImageFormat::R8G8B8A8_UNORM:
	case API::ImageFormat::R16G16B16A16_UNORM:
	case API::ImageFormat::R32G32B32A32_SFLOAT:
		return Image::ColorFormat::RGBA;

	default:
		return Image::ColorFormat::NONE;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetBitsPerChannelFromImageFormat(const API::ImageFormat imageFormat)
{
	switch(imageFormat)
	{
	case API::ImageFormat::R8_UNORM:
	case API::ImageFormat::R8G8_UNORM:
	case API::ImageFormat::R8G8B8A8_UNORM:
		return 8;

	case API::ImageFormat::R16_UNORM:
	case API::ImageFormat::R16G16_UNORM:
	case API::ImageFormat::R16G16B16A16_UNORM:
		return 16;

	case API::ImageFormat::R32_SFLOAT:
	case API::ImageFormat::R32G32_SFLOAT:
	case API::ImageFormat::R32G32B32A32_SFLOAT:
		return 32;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Graphics::Texture::Rotate90Clockwise(const TRAP::Image* const img)
{
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
	if(img->GetBitsPerChannel() == 32)
	{
		std::vector<float> rotated(static_cast<std::size_t>(img->GetWidth()) * img->GetHeight() *
								   static_cast<std::size_t>(img->GetColorFormat()));
		std::memcpy(rotated.data(), img->GetPixelData(), img->GetPixelDataSize());
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
		std::memcpy(rotated.data(), img->GetPixelData(), img->GetPixelDataSize());
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
	std::memcpy(rotated.data(), img->GetPixelData(), img->GetPixelDataSize());
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
	//Hot Reloading
	if(TRAP::Application::IsHotReloadingEnabled())
	{
		for(const std::filesystem::path& path : m_filepaths)
		{
			if(path.empty())
				continue;

			TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(FS::GetFolderPath(path));
		}
	}
}
