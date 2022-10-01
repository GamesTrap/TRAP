#include "TRAPPCH.h"
#include "RenderTarget.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/API/Vulkan/Objects/VulkanRenderTarget.h"

TRAP::Graphics::RenderTarget::RenderTarget()
	: m_texture(nullptr), m_clearColor(), m_clearDepth(0.0f), m_clearStencil(0), m_arraySize(),
	  m_depth(), m_width(), m_height(), m_descriptors(), m_mipLevels(), m_sampleQuality(), m_format(),
	  m_sampleCount()
{
	ZoneScoped;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Creating RenderTarget");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderTarget::~RenderTarget()
{
	ZoneScoped;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Destroying RenderTarget");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::RenderTarget> TRAP::Graphics::RenderTarget::Create(const RendererAPI::RenderTargetDesc& desc)
{
	ZoneScoped;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanRenderTarget>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetWidth() const
{
	ZoneScoped;

	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetHeight() const
{
	ZoneScoped;

	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetDepth() const
{
	ZoneScoped;

	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetArraySize() const
{
	ZoneScoped;

	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetMipLevels() const
{
	ZoneScoped;

	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RenderTarget::GetSampleCount() const
{
	ZoneScoped;

	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetSampleQuality() const
{
	ZoneScoped;

	return m_sampleQuality;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::RenderTarget::GetImageFormat() const
{
	ZoneScoped;

	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec4 TRAP::Graphics::RenderTarget::GetClearColor() const
{
	ZoneScoped;

	return m_clearColor;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::RenderTarget::GetClearDepth() const
{
	ZoneScoped;

	return m_clearDepth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetClearStencil() const
{
	ZoneScoped;

	return m_clearStencil;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::RenderTarget::GetDescriptorType() const
{
	ZoneScoped;

	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::RenderTarget::GetTexture() const
{
	ZoneScoped;

	return m_texture;
}