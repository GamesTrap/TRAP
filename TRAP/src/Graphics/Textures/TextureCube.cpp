#include "TRAPPCH.h"
#include "TextureCube.h"

#include "Graphics/API/RendererAPI.h"
#include "FS/FS.h"
#include "Graphics/API/ResourceLoader.h"
#include "TextureBase.h"

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFiles(const std::string& name,
	                                                                                  const std::array<std::filesystem::path, 6>& filepaths)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_ERROR(Log::TextureCubePrefix, "Invalid name!");
		return nullptr;
	}

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube(name, filepaths,
		                                                                            TextureCubeFormat::MultiFile));

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths = filepaths;
		desc.IsCubemap = true;
		desc.Type = RendererAPI::TextureCubeType::MultiFile;
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);

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
	                                                                                 const std::filesystem::path& filepath,
																					 const TextureCubeFormat format)
{
	TRAP_ASSERT(format != TextureCubeFormat::MultiFile,
	            "CreateFromFile can not be used with TextureCubeFormat::MultiFile!");

	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::TextureCubePrefix, "Name is empty! Using filename as name!");
		return CreateFromFile(filepath, format);
	}

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube(name, filepath, format));

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = true;
		/*desc.Type = format == TextureCubeFormat::Cross ? RendererAPI::TextureCubeType::Cross :
		              RendererAPI::TextureCubeType::Equirectangular;*/
		desc.Type = RendererAPI::TextureCubeType::Cross; //TODO Replace when Equirectangular is implemented
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);

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

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::CreateFromFile(const std::filesystem::path& filepath,
	                                                                                 const TextureCubeFormat format)
{
	TRAP_ASSERT(format != TextureCubeFormat::MultiFile,
	            "CreateFromFile can not be used with TextureCubeFormat::MultiFile!");

	TP_PROFILE_FUNCTION();

	const std::string name = FS::GetFileName(filepath);

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube(name, filepath, format));

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = true;
		/*desc.Type = format == TextureCubeFormat::Cross ? RendererAPI::TextureCubeType::Cross :
		              RendererAPI::TextureCubeType::Equirectangular;*/
		desc.Type = RendererAPI::TextureCubeType::Cross; //TODO Replace when Equirectangular is implemented
		desc.Texture = &texture->m_texture;

		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(desc, &texture->m_syncToken);

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
																					  const TextureCubeFormat format)
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
		const API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(useImg->GetColorFormat(), useImg->GetBitsPerPixel());

		if(imageFormat == API::ImageFormat::Undefined)
			return nullptr;

		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube(name, useImg->GetFilePath(),
		                                                                            format));

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

		//Wait for texture to be ready
		RendererAPI::GetResourceLoader()->WaitForToken(&texture->m_syncToken);

		if(format == TextureCubeFormat::Cross)
		{
			//Fill empty TextureBase with images pixel data
			for(uint32_t i = 0; i < faces.size(); ++i)
			{
				RendererAPI::TextureUpdateDesc updateDesc{};
				updateDesc.Texture = texture->m_texture;
				updateDesc.ArrayLayer = i;
				TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(updateDesc);
				if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single memcpy is enough
					memcpy(updateDesc.MappedData, faces[i]->GetPixelData(), faces[i]->GetPixelDataSize());
				else //Needs row by row copy
				{
					for(std::size_t r = 0; r < updateDesc.RowCount; ++r)
					{
						memcpy(updateDesc.MappedData + r * updateDesc.DstRowStride,
							static_cast<const uint8_t*>(faces[i]->GetPixelData()) + r * updateDesc.SrcRowStride,
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
	                                                                                   const std::array<Scope<Image>, 6>& imgs)
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
		TP_ERROR(Log::TextureCubePrefix, "An image has mismatching color format, bits per channel, width and/or height!");
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
		const API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(useImgs[0]->GetColorFormat(),
		                                                                          useImgs[0]->GetBitsPerPixel());

		if(imageFormat == API::ImageFormat::Undefined)
			return nullptr;

		std::array<std::filesystem::path, 6> filepaths{};
		for(uint32_t i = 0; i < imgs.size(); ++i)
			filepaths[i] = useImgs[i]->GetFilePath();

		TRAP::Scope<TextureCube> texture = TRAP::Scope<TextureCube>(new TextureCube(name, filepaths,
		                                                                            TextureCubeFormat::MultiFile));

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

		//Wait for texture to be ready
		RendererAPI::GetResourceLoader()->WaitForToken(&texture->m_syncToken);

		//Fill empty TextureBase with images pixel data
		for(uint32_t i = 0; i < imgs.size(); ++i)
		{
			RendererAPI::TextureUpdateDesc updateDesc{};
			updateDesc.Texture = texture->m_texture;
			updateDesc.ArrayLayer = i;
			TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(updateDesc);
			if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single memcpy is enough
				memcpy(updateDesc.MappedData, useImgs[i]->GetPixelData(), useImgs[i]->GetPixelDataSize());
			else //Needs row by row copy
			{
				for(std::size_t r = 0; r < updateDesc.RowCount; ++r)
				{
					memcpy(updateDesc.MappedData + r * updateDesc.DstRowStride,
						static_cast<const uint8_t*>(useImgs[i]->GetPixelData()) + r * updateDesc.SrcRowStride,
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

TRAP::Scope<TRAP::Graphics::TextureCube> TRAP::Graphics::TextureCube::Create()
{
	std::array<TRAP::Scope<TRAP::Image>, 6> imgs{};
	for(uint32_t i = 0; i < imgs.size(); ++i)
		imgs[i] = TRAP::Image::LoadFallback();

	return CreateFromImages("FallbackCube", imgs);
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<std::filesystem::path, 6> TRAP::Graphics::TextureCube::GetFilePaths() const
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

void TRAP::Graphics::TextureCube::Update(const void* data, const uint32_t sizeInBytes,
                                         const uint32_t mipLevel, const uint32_t arrayLayer)
{
	TRAP_ASSERT(data, "Update: Data is nullptr!");
	TRAP_ASSERT(arrayLayer < 6, "Invalid array layer provided!");
	TRAP_ASSERT(mipLevel < m_texture->GetMipLevels(), "Invalid mip level provided!");
	TRAP_ASSERT(sizeInBytes >= (m_texture->GetWidth() >> mipLevel) * (m_texture->GetHeight() >> mipLevel) *
	            GetBytesPerPixel(), "Texture update size is too small");

	if(arrayLayer >= 6)
	{
		TP_ERROR(Log::TextureCubePrefix, "Update: Invalid array layer provided!");
		return;
	}
	if(mipLevel >= m_texture->GetMipLevels())
	{
		TP_ERROR(Log::TextureCubePrefix, "Update: Invalid mip level provided!");
		return;
	}
	if(sizeInBytes < (m_texture->GetWidth() >> mipLevel) * (m_texture->GetHeight() >> mipLevel) *
	   GetBytesPerPixel())
	{
		TP_ERROR(Log::TextureCubePrefix, "Update: Texture update size is too small!");
		return;
	}

	RendererAPI::TextureUpdateDesc updateDesc{};
	updateDesc.Texture = m_texture;
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

TRAP::Graphics::TextureCube::TextureCube(std::string name, std::array<std::filesystem::path, 6> filepaths,
										 const TextureCubeFormat format)
	: m_textureFormat(format)
{
	m_textureType = TextureType::TextureCube;
	m_name = std::move(name);
	m_filepaths = std::move(filepaths);

	//Hot reloading
	if(!TRAP::Application::IsHotReloadingEnabled())
		return;
	FS::FileWatcher* fw = TRAP::Application::GetHotReloadingFileWatcher();
	for(const auto& filepath : m_filepaths)
	{
		if(!filepath.empty())
			fw->AddFolder(FS::GetFolderPath(filepath));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureCube::TextureCube(std::string name, std::filesystem::path filepath,
										 const TextureCubeFormat format)
	: m_textureFormat(format)
{
	m_textureType = TextureType::TextureCube;
	m_name = std::move(name);
	m_filepaths[0] = std::move(filepath);

	//Hot reloading
	if(!TRAP::Application::IsHotReloadingEnabled())
		return;
	TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(FS::GetFolderPath(m_filepaths[0]));
}