#include "TRAPPCH.h"
#include "Buffer.h"

#include "Graphics/API/Vulkan/Objects/VulkanBuffer.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Buffer> TRAP::Graphics::Buffer::Create(const RendererAPI::BufferDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanBuffer>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Buffer::Create(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Buffer::Buffer(const uint64_t size, const RendererAPI::DescriptorType descriptorType,
                               const RendererAPI::ResourceMemoryUsage memoryUsage)
	: m_size(size), m_descriptors(descriptorType), m_memoryUsage(memoryUsage)
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererBufferPrefix, "Creating Buffer");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Buffer::~Buffer()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererBufferPrefix, "Destroying Buffer");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}