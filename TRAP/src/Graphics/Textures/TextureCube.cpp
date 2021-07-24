#include "TRAPPCH.h"
#include "TextureCube.h"

#include "Graphics/API/RendererAPI.h"
#include "VFS/VFS.h"
#include "Graphics/API/ResourceLoader.h"
#include "TextureBase.h"

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFiles(const std::string& name,
	                                                                                  const std::array<std::string, 6>& filepaths,
																					  const TextureUsage usage)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_ERROR(Log::TextureCubePrefix, "Invalid Name!");
		return nullptr;
	}

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = filepaths;
		desc.IsCubemap = true;
		desc.Type = RendererAPI::TextureCubeType::MultiFile;
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
		texture->m_name = name;
		texture->m_filepaths = filepaths;
		texture->m_textureFormat = TextureCubeFormat::MultiFile;
		texture->m_textureType = TextureType::TextureCube;
		texture->m_textureUsage = usage;

		return texture;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFile(const std::string& name,
	                                                                                 const std::string& filepath,
																					 const TextureCubeFormat format,
																					 const TextureUsage usage)
{
	TRAP_ASSERT(format != TextureCubeFormat::MultiFile,
	            "CreateFromFile can not be used with TextureCubeFormat::MultiFile!");

	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::TextureCubePrefix, "Name is empty! Using Filename as TextureCube Name!");
		return CreateFromFile(filepath, format, usage);
	}

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = true;
		/*desc.Type = format == TextureCubeFormat::Cross ? RendererAPI::TextureCubeType::Cross :
		              RendererAPI::TextureCubeType::Equirectangular;*/
		desc.Type = RendererAPI::TextureCubeType::Cross; //TODO Replace when Equirectangular is implemented
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
		texture->m_name = name;
		texture->m_filepaths[0] = filepath;
		texture->m_textureFormat = format;
		texture->m_textureType = TextureType::TextureCube;
		texture->m_textureUsage = usage;

		return texture;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFile(const std::string& filepath,
	                                                                                 const TextureCubeFormat format,
																					 const TextureUsage usage)
{
	TRAP_ASSERT(format != TextureCubeFormat::MultiFile,
	            "CreateFromFile can not be used with TextureCubeFormat::MultiFile!");

	TP_PROFILE_FUNCTION();

	std::string name = VFS::GetFileName(VFS::MakeVirtualPathCompatible(filepath));

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube());

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = true;
		/*desc.Type = format == TextureCubeFormat::Cross ? RendererAPI::TextureCubeType::Cross :
		              RendererAPI::TextureCubeType::Equirectangular;*/
		desc.Type = RendererAPI::TextureCubeType::Cross; //TODO Replace when Equirectangular is implemented
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);
		texture->m_name = name;
		texture->m_filepaths[0] = filepath;
		texture->m_textureFormat = format;
		texture->m_textureType = TextureType::TextureCube;
		texture->m_textureUsage = usage;

		return texture;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromImage(const std::string& name,
	                                                                                  const Scope<Image>& img,
																					  const TextureCubeFormat format,
																					  const TextureUsage usage)
{
	TRAP_ASSERT(img, "Image is nullptr!");
	TRAP_ASSERT(format != TextureCubeFormat::MultiFile,
	            "CreateFromImage can not be used with TextureCubeFormat::MultiFile!");

	TP_PROFILE_FUNCTION();

	TRAP::Scope<Image> imgRGBA = nullptr;
	if(img->GetColorFormat() == Image::ColorFormat::RGB)
		imgRGBA = TRAP::Image::ConvertRGBToRGBA(img);

	Image* useImg = imgRGBA ? imgRGBA.get() : img.get();

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(useImg->GetColorFormat(), useImg->GetBitsPerPixel());

		if(imageFormat == API::ImageFormat::Undefined)
			return nullptr;

		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube());

		TRAP::Graphics::RendererAPI::TextureLoadDesc loadDesc{};

		std::array<TRAP::Scope<Image>, 6> faces{};
		if(format == TextureCubeFormat::Cross)
		{
			if(useImg->GetBytesPerChannel() == 1)
				faces = SplitImageFromCross<uint8_t>(imgRGBA ? imgRGBA : img);
			else if(useImg->GetBytesPerChannel() == 2)
				faces = SplitImageFromCross<uint16_t>(imgRGBA ? imgRGBA : img);
			else if(useImg->GetBytesPerChannel() == 4)
				faces = SplitImageFromCross<float>(imgRGBA ? imgRGBA : img);

			loadDesc.Desc->MipLevels = CalculateMipLevels(faces[0]->GetWidth(), faces[0]->GetHeight());
		}

		//Create empty TextureBase
		loadDesc.IsCubemap = true;
		loadDesc.Type = RendererAPI::TextureCubeType::Cross;
		loadDesc.Desc = TRAP::MakeRef<RendererAPI::TextureDesc>();
		loadDesc.Desc->Width = useImg->GetWidth();
		loadDesc.Desc->Height = useImg->GetHeight();
		loadDesc.Desc->ArraySize = 6;
		loadDesc.Desc->Format = imageFormat;
		loadDesc.Desc->StartState = RendererAPI::ResourceState::Common;
		loadDesc.Desc->Descriptors = RendererAPI::DescriptorType::Texture | RendererAPI::DescriptorType::TextureCube;
		loadDesc.Texture = &texture->m_texture;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(loadDesc, &texture->m_syncToken);

		//Set TextureCube data
		texture->m_name = name;
		texture->m_textureFormat = format;
		texture->m_filepaths[0] = useImg->GetFilePath();
		texture->m_textureType = TextureType::TextureCube;
		texture->m_textureUsage = usage;

		//Wait for texture to be ready
		RendererAPI::GetResourceLoader()->WaitForToken(&texture->m_syncToken);

		if(format == TextureCubeFormat::Cross)
		{
			//Fill empty TextureBase with images pixel data
			for(uint32_t i = 0; i < 6; ++i)
			{
				RendererAPI::TextureUpdateDesc updateDesc{};
				updateDesc.Texture = texture->m_texture;
				updateDesc.ArrayLayer = i;
				TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(updateDesc);
				if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single memcpy is enough
					memcpy(updateDesc.MappedData, faces[i]->GetPixelData(), faces[i]->GetPixelDataSize());
				else //Needs row by row copy
				{
					for(uint32_t r = 0; r < updateDesc.RowCount; ++r)
					{
						memcpy(updateDesc.MappedData + r * updateDesc.DstRowStride,
							reinterpret_cast<const uint8_t*>(faces[i]->GetPixelData()) + r * updateDesc.SrcRowStride,
							updateDesc.SrcRowStride);
					}
				}
				TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &texture->m_syncToken);
			}
		}
		//else /*if(format == TextureCubeFormat::Equirectangular)*/

		return texture;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromImages(const std::string& name,
	                                                                                   const std::array<Scope<Image>, 6>& imgs,
																					   const TextureUsage usage)
{
	TRAP_ASSERT(std::none_of(imgs.cbegin(), imgs.cend(),
	                        [](const Scope<Image>& img) { return img.get() == nullptr; }), "An Image is nullptr!");

	TP_PROFILE_FUNCTION();

	//Validation that images have same size and format
	Image::ColorFormat format = imgs[0]->GetColorFormat();
	if(std::none_of(imgs.cbegin(), imgs.cend(),
	   [&](const Scope<Image>& img) {return img->GetColorFormat() != format ||
	                                                   img->GetBitsPerChannel() != imgs[0]->GetBitsPerPixel() ||
													   img->GetWidth() != imgs[0]->GetWidth() ||
													   img->GetHeight() != imgs[0]->GetHeight(); }))
	{
		TP_ERROR(Log::TextureCubePrefix, "An Image has mismatching ColorFormat, Bits Per Channel, Width and/or Height!");
		return nullptr;
	}

	//Convert to RGBA if necessary
	std::array<Scope<Image>, 6> imgsRGBA{};
	if(format == Image::ColorFormat::RGB)
	{
		for(uint32_t i = 0; i < imgs.size(); ++i)
			imgsRGBA[i] = TRAP::Image::ConvertRGBToRGBA(imgs[i]);
	}

	const Scope<Image>* useImgs = (format == Image::ColorFormat::RGB ? imgsRGBA.data() : imgs.data());

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(useImgs[0]->GetColorFormat(),
		                                                                    useImgs[0]->GetBitsPerPixel());

		if(imageFormat == API::ImageFormat::Undefined)
			return nullptr;

		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube());

		//Create empty TextureBase
		TRAP::Graphics::RendererAPI::TextureLoadDesc loadDesc{};
		loadDesc.IsCubemap = true;
		loadDesc.Type = RendererAPI::TextureCubeType::MultiFile;
		loadDesc.Desc = TRAP::MakeRef<RendererAPI::TextureDesc>();
		loadDesc.Desc->Width = useImgs[0]->GetWidth();
		loadDesc.Desc->Height = useImgs[0]->GetHeight();
		loadDesc.Desc->ArraySize = 6;
		loadDesc.Desc->MipLevels = CalculateMipLevels(useImgs[0]->GetWidth(), useImgs[0]->GetHeight());
		loadDesc.Desc->Format = imageFormat;
		loadDesc.Desc->StartState = RendererAPI::ResourceState::Common;
		loadDesc.Desc->Descriptors = RendererAPI::DescriptorType::Texture | RendererAPI::DescriptorType::TextureCube;
		loadDesc.Texture = &texture->m_texture;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(loadDesc, &texture->m_syncToken);

		//Set TextureCube data
		texture->m_name = name;
		texture->m_textureFormat = TextureCubeFormat::MultiFile;
		for(uint32_t i = 0; i < imgs.size(); ++i)
			texture->m_filepaths[i] = useImgs[i]->GetFilePath();
		texture->m_textureType = TextureType::TextureCube;
		texture->m_textureUsage = usage;

		//Wait for texture to be ready
		RendererAPI::GetResourceLoader()->WaitForToken(&texture->m_syncToken);

		//Fill empty TextureBase with images pixel data
		for(uint32_t i = 0; i < 6; ++i)
		{
			RendererAPI::TextureUpdateDesc updateDesc{};
			updateDesc.Texture = texture->m_texture;
			updateDesc.ArrayLayer = i;
			TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(updateDesc);
			if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single memcpy is enough
				memcpy(updateDesc.MappedData, useImgs[i]->GetPixelData(), useImgs[i]->GetPixelDataSize());
			else //Needs row by row copy
			{
				for(uint32_t r = 0; r < updateDesc.RowCount; ++r)
				{
					memcpy(updateDesc.MappedData + r * updateDesc.DstRowStride,
						reinterpret_cast<const uint8_t*>(useImgs[i]->GetPixelData()) + r * updateDesc.SrcRowStride,
						updateDesc.SrcRowStride);
				}
			}
			TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &texture->m_syncToken);
		}

		return texture;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::Create(const TextureUsage usage)
{
	std::array<TRAP::Scope<TRAP::Image>, 6> imgs{};
	for(uint32_t i = 0; i < imgs.size(); ++i)
		imgs[i] = TRAP::Image::LoadFallback();

	return CreateFromImages("FallbackCube", imgs, usage);
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<std::string, 6> TRAP::Graphics::TextureCube::GetFilePaths() const
{
	return m_filepaths;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCubeFormat TRAP::Graphics::TextureCube::GetTextureCubeFormat() const
{
	return m_textureFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::TextureCube::GetDepth() const
{
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::TextureCube::GetArraySize() const
{
	return 6;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureCube::Update(const void* data, const uint32_t sizeInBytes, uint32_t mipLevel,
										 uint32_t arrayLayer)
{
	TRAP_ASSERT(arrayLayer < 6, "Invalid Arraylayer provided!");
	TRAP_ASSERT(mipLevel < m_texture->GetMipLevels(), "Invalid Miplevel provided!");
	TRAP_ASSERT(sizeInBytes >= (m_texture->GetWidth() >> mipLevel) * (m_texture->GetHeight() >> mipLevel) *
	            GetBytesPerPixel(), "Texture update size is too small");

	RendererAPI::TextureUpdateDesc updateDesc{};
	updateDesc.Texture = m_texture;
	updateDesc.MipLevel = mipLevel;
	updateDesc.ArrayLayer = arrayLayer;
	TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(updateDesc);
	if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single memcpy is enough
		memcpy(updateDesc.MappedData, data, updateDesc.RowCount * updateDesc.SrcRowStride);
	else //Needs row by row copy
	{
		for(uint32_t r = 0; r < updateDesc.RowCount; ++r)
		{
			memcpy(updateDesc.MappedData + r * updateDesc.DstRowStride,
				   reinterpret_cast<const uint8_t*>(data) + r * updateDesc.SrcRowStride,
				   updateDesc.SrcRowStride);
		}
	}
	TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCube::TextureCube()
	: m_textureFormat(TextureCubeFormat::Cross)
{
	m_textureType = TextureType::TextureCube;
}