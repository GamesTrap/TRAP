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
{}

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