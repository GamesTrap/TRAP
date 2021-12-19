#include "TRAPPCH.h"
#include "RenderTarget.h"

#include "Graphics/API/Vulkan/Objects/VulkanRenderTarget.h"

TRAP::Graphics::RenderTarget::RenderTarget()
	: m_texture(nullptr),
	  m_clearColor(),
	  m_clearDepth(1.0f),
	  m_clearStencil(0),
	  m_arraySize(),
	  m_depth(),
	  m_width(),
	  m_height(),
	  m_descriptors(),
	  m_mipLevels(),
	  m_sampleQuality(),
	  m_format(),
	  m_sampleCount()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Creating RenderTarget");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderTarget::~RenderTarget()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Destroying RenderTarget");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::RenderTarget> TRAP::Graphics::RenderTarget::Create(const RendererAPI::RenderTargetDesc& desc)
{
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
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetDepth() const
{
	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetArraySize() const
{
	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetMipLevels() const
{
	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RenderTarget::GetSampleCount() const
{
	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetSampleQuality() const
{
	return m_sampleQuality;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::RenderTarget::GetImageFormat() const
{
	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec4 TRAP::Graphics::RenderTarget::GetClearColor() const
{
	return m_clearColor;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::RenderTarget::GetClearDepth() const
{
	return m_clearDepth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetClearStencil() const
{
	return m_clearStencil;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::RenderTarget::GetDescriptorType() const
{
	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::TextureBase> TRAP::Graphics::RenderTarget::GetTexture() const
{
	return m_texture;
}