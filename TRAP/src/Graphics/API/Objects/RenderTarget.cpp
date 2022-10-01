#include "TRAPPCH.h"
#include "RenderTarget.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/API/Vulkan/Objects/VulkanRenderTarget.h"

TRAP::Graphics::RenderTarget::RenderTarget()
	: m_texture(nullptr), m_clearColor(), m_clearDepth(0.0f), m_clearStencil(0), m_arraySize(),
	  m_depth(), m_width(), m_height(), m_descriptors(), m_mipLevels(), m_sampleQuality(), m_format(),
	  m_sampleCount()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Creating RenderTarget");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderTarget::~RenderTarget()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Destroying RenderTarget");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::RenderTarget> TRAP::Graphics::RenderTarget::Create(const RendererAPI::RenderTargetDesc& desc)
{
	ZoneScopedC(tracy::Color::Red);

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
	ZoneScopedC(tracy::Color::Red);

	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetHeight() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetDepth() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetArraySize() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetMipLevels() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RenderTarget::GetSampleCount() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetSampleQuality() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_sampleQuality;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::RenderTarget::GetImageFormat() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec4 TRAP::Graphics::RenderTarget::GetClearColor() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_clearColor;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::RenderTarget::GetClearDepth() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_clearDepth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderTarget::GetClearStencil() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_clearStencil;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::RenderTarget::GetDescriptorType() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::RenderTarget::GetTexture() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_texture;
}