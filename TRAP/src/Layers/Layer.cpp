#include "TRAPPCH.h"
#include "Layer.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Layer::Layer(std::string debugName) noexcept
	: m_DebugName(std::move(debugName))
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnAttach()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnDetach()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnUpdate([[maybe_unused]] const Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnTick([[maybe_unused]] const Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnImGuiRender()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnEvent([[maybe_unused]] Events::Event& event)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Layer::GetName() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_DebugName;
}