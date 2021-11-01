#include "TRAPPCH.h"
#include "Texture2D.h"

#include "Graphics/API/RendererAPI.h"
#include "VFS/VFS.h"
#include "Graphics/API/ResourceLoader.h"
#include "TextureBase.h"

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string& name,
	                                                                             const std::string_view filepath)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::Texture2DPrefix, "Name is empty! Using Filename as Texture2D Name!");
		return CreateFromFile(filepath);
	}

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<Texture2D> texture = TRAP::Scope<Texture2D>(new Texture2D(name, filepath));

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = false;
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

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromFile(const std::string_view filepath)
{
	TP_PROFILE_FUNCTION();

	std::string name = VFS::GetFileName(VFS::MakeVirtualPathCompatible(filepath));

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Scope<Texture2D> texture = TRAP::Scope<Texture2D>(new Texture2D(name, filepath));

		//Load Texture
		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Filepaths[0] = filepath;
		desc.IsCubemap = false;
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

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateFromImage(const std::string& name,
	                                                                              const Scope<Image>& img)
{
	TRAP_ASSERT(img, "Image is nullptr!");

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

		TRAP::Scope<Texture2D> texture = TRAP::Scope<Texture2D>(new Texture2D(name, useImg->GetFilePath()));

		//Create empty TextureBase
		TRAP::Graphics::RendererAPI::TextureLoadDesc loadDesc{};
		loadDesc.IsCubemap = false;
		loadDesc.Desc = TRAP::MakeRef<RendererAPI::TextureDesc>();
		loadDesc.Desc->Width = useImg->GetWidth();
		loadDesc.Desc->Height = useImg->GetHeight();
		loadDesc.Desc->MipLevels = CalculateMipLevels(useImg->GetWidth(), useImg->GetHeight());
		loadDesc.Desc->Format = imageFormat;
		loadDesc.Desc->StartState = RendererAPI::ResourceState::Common;
		loadDesc.Desc->Descriptors = RendererAPI::DescriptorType::Texture;
		loadDesc.Texture = &texture->m_texture;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(loadDesc, &texture->m_syncToken);

		//Wait for texture to be ready
		RendererAPI::GetResourceLoader()->WaitForToken(&texture->m_syncToken);

		//Fill empty TextureBase with images pixel data
		RendererAPI::TextureUpdateDesc updateDesc{};
		updateDesc.Texture = texture->m_texture;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(updateDesc);
		if(updateDesc.DstRowStride == updateDesc.SrcRowStride) //Single memcpy is enough
			memcpy(updateDesc.MappedData, useImg->GetPixelData(), useImg->GetPixelDataSize());
		else //Needs row by row copy
		{
			for(std::size_t r = 0; r < updateDesc.RowCount; ++r)
			{
				memcpy(updateDesc.MappedData + r * updateDesc.DstRowStride,
				       reinterpret_cast<const uint8_t*>(useImg->GetPixelData()) + r * updateDesc.SrcRowStride,
					   updateDesc.SrcRowStride);
			}
		}
		TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &texture->m_syncToken);

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

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::CreateEmpty(uint32_t width, uint32_t height,
	                                                                          uint32_t bitsPerPixel,
																			  Image::ColorFormat format)
{
	TP_PROFILE_FUNCTION();

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		API::ImageFormat imageFormat = ColorFormatBitsPerPixelToImageFormat(format, bitsPerPixel);

		if(imageFormat == API::ImageFormat::Undefined)
			return nullptr;

		TRAP::Scope<Texture2D> texture = TRAP::Scope<Texture2D>(new Texture2D());

		TRAP::Graphics::RendererAPI::TextureLoadDesc desc{};
		desc.Desc = TRAP::MakeRef<RendererAPI::TextureDesc>();
		desc.Desc->Width = width;
		desc.Desc->Height = height;
		desc.Desc->Format = imageFormat;
		desc.Desc->Descriptors = RendererAPI::DescriptorType::Texture;
		desc.Desc->StartState = RendererAPI::ResourceState::Common;
		desc.IsCubemap = false;
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

TRAP::Scope<TRAP::Graphics::Texture2D> TRAP::Graphics::Texture2D::Create()
{
	return CreateFromImage("Fallback2D", TRAP::Image::LoadFallback());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture2D::GetDepth() const
{
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture2D::GetArraySize() const
{
	return m_texture->GetArraySize();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Texture2D::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture2D::Update(const void* data, const uint32_t sizeInBytes, const uint32_t mipLevel,
 									   const uint32_t arrayLayer)
{
	TRAP_ASSERT(data, "Update: Data is nullptr!");
	TRAP_ASSERT(arrayLayer < m_texture->GetArraySize(), "Invalid Arraylayer provided!");
	TRAP_ASSERT(mipLevel < m_texture->GetMipLevels(), "Invalid Miplevel provided!");
	TRAP_ASSERT(sizeInBytes >= (m_texture->GetWidth() >> mipLevel) * (m_texture->GetHeight() >> mipLevel) *
	            GetBytesPerPixel(), "Texture update size is too small");

	if(arrayLayer >= 6)
	{
		TP_ERROR(Log::Texture2DPrefix, "Update: Invalid Arraylayer provided!");
		return;
	}
	if(mipLevel >= m_texture->GetMipLevels())
	{
		TP_ERROR(Log::Texture2DPrefix, "Update: Invalid MipLevel provided!");
		return;
	}
	if(sizeInBytes < (m_texture->GetWidth() >> mipLevel) * (m_texture->GetHeight() >> mipLevel) *
	   GetBytesPerPixel())
	{
		TP_ERROR(Log::Texture2DPrefix, "Update: Texture update size is too small!");
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
				   reinterpret_cast<const uint8_t*>(data) + r * updateDesc.SrcRowStride,
				   updateDesc.SrcRowStride);
		}
	}
	TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(updateDesc, &m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture2D::Texture2D()
{
	m_textureType = TextureType::Texture2D;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture2D::Texture2D(const std::string& name, std::string_view filepath)
{
	m_textureType = TextureType::Texture2D;
	m_name = name;
	m_filepath = filepath;
}