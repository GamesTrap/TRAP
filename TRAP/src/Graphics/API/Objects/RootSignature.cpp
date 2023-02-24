#include "TRAPPCH.h"
#include "RootSignature.h"

#include "Graphics/API/Vulkan/Objects/VulkanRootSignature.h"

TRAP::Graphics::RootSignature::RootSignature()
	: m_pipelineType(), m_descriptorNameToIndexMap()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRootSignaturePrefix, "Creating RootSignature");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RootSignature::~RootSignature()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRootSignaturePrefix, "Destroying RootSignature");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::PipelineType TRAP::Graphics::RootSignature::GetPipelineType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_pipelineType;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::RootSignature::GetDescriptorCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return static_cast<uint32_t>(m_descriptors.size());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::vector<TRAP::Graphics::RendererAPI::DescriptorInfo>& TRAP::Graphics::RootSignature::GetDescriptors() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Graphics::RendererAPI::DescriptorIndexMap& TRAP::Graphics::RootSignature::GetDescriptorNameToIndexMap() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_descriptorNameToIndexMap;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::RootSignature::Create(const RendererAPI::RootSignatureDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanRootSignature>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "RootSignature::Create(): Unknown RenderAPI");
		return nullptr;
	}
}