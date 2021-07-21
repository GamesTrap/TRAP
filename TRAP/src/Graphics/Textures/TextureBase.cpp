#include "TRAPPCH.h"
#include "TextureBase.h"

#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Ref<TRAP::Graphics::TextureBase> TRAP::Graphics::TextureBase::Create(const RendererAPI::TextureDesc& desc)
{
    TRAP_ASSERT(desc.Width && desc.Height && (desc.Depth || desc.ArraySize));

    if(desc.SampleCount > RendererAPI::SampleCount::SampleCount1 && desc.MipLevels > 1)
	{
		TP_ERROR(Log::TextureBasePrefix, "Multi-Sampled textures cannot have mip maps");
		TRAP_ASSERT(false);
		return nullptr;
	}

    switch(TRAP::Graphics::RendererAPI::GetRenderAPI())
    {
    case RenderAPI::Vulkan:
    {
        const auto* vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>(RendererAPI::GetRenderer().get());
        TRAP::Ref<TextureBase> texture = TRAP::MakeRef<API::VulkanTexture>(vkRenderer->GetDevice(), desc,
                                                                           vkRenderer->GetVMA());

        texture->m_width = desc.Width;
        texture->m_height = desc.Height;
        texture->m_depth = desc.Depth;
        texture->m_mipLevels = desc.MipLevels;
        texture->m_arraySize = desc.ArraySize;
        texture->m_format = desc.Format;
        texture->m_UAV = desc.Descriptors & RendererAPI::DescriptorType::RWTexture;

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

uint32_t TRAP::Graphics::TextureBase::GetWidth() const
{
    return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::TextureBase::GetHeight() const
{
    return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::TextureBase::GetDepth() const
{
    return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::TextureBase::GetMipLevels() const
{
    return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::TextureBase::GetArraySize() const
{
    return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::TextureBase::GetImageFormat() const
{
    return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::TextureBase::GetAspectMask() const
{
    return m_aspectMask;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::TextureBase::GetUAV() const
{
    return m_UAV;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::TextureBase::OwnsImage() const
{
    return m_ownsImage;
}