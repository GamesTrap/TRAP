#include "TRAPPCH.h"
#include "Buffer.h"

#include "Graphics/API/Vulkan/Objects/VulkanBuffer.h"
#include "Graphics/API/RendererAPI.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Buffer> TRAP::Graphics::Buffer::Create(const BufferDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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
