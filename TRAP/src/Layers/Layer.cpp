#include "TRAPPCH.h"
#include "Layer.h"

void TRAP::Layer::OnAttach()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None && (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnDetach()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None && (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnUpdate([[maybe_unused]] const Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnTick([[maybe_unused]] const Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnImGuiRender()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnEvent([[maybe_unused]] Events::Event& event)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
}
