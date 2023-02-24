#include "TRAPPCH.h"
#include "Buffer.h"

#include "Graphics/API/Vulkan/Objects/VulkanBuffer.h"

TRAP::Graphics::Buffer::Buffer()
	: m_CPUMappedAddress(nullptr), m_size(), m_descriptors(), m_memoryUsage()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererBufferPrefix, "Creating Buffer");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Buffer::~Buffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererBufferPrefix, "Destroying Buffer");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t TRAP::Graphics::Buffer::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::Buffer::GetDescriptors() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::ResourceMemoryUsage TRAP::Graphics::Buffer::GetMemoryUsage() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_memoryUsage;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* TRAP::Graphics::Buffer::GetCPUMappedAddress() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_CPUMappedAddress;
}

//-------------------------------------------------------------------------------------------------------------------//

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