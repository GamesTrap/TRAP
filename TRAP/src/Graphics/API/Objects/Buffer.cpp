#include "TRAPPCH.h"
#include "Buffer.h"

#include "Graphics/API/Vulkan/Objects/VulkanBuffer.h"

TRAP::Graphics::Buffer::Buffer()
	: m_CPUMappedAddress(nullptr), m_size(), m_descriptors(), m_memoryUsage()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererBufferPrefix, "Creating Buffer");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Buffer::~Buffer()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererBufferPrefix, "Destroying Buffer");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Buffer> TRAP::Graphics::Buffer::Create(const RendererAPI::BufferDesc& desc)
{
	ZoneScopedC(tracy::Color::Red);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanBuffer>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}