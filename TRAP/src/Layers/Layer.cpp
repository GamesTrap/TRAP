#include "TRAPPCH.h"
#include "Layer.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Layer::Layer(std::string debugName)
	: m_DebugName(std::move(debugName))
{
	ZoneScopedC(tracy::Color::Brown);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnAttach()
{
	ZoneScopedC(tracy::Color::Brown);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnDetach()
{
	ZoneScopedC(tracy::Color::Brown);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnUpdate(const Utils::TimeStep& /*deltaTime*/)
{
	ZoneScopedC(tracy::Color::Brown);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnTick(const Utils::TimeStep& /*deltaTime*/)
{
	ZoneScopedC(tracy::Color::Brown);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnImGuiRender()
{
	ZoneScopedC(tracy::Color::Brown);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnEvent(Events::Event& /*event*/)
{
	ZoneScopedC(tracy::Color::Brown);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Layer::GetName() const
{
	ZoneScopedC(tracy::Color::Brown);

	return m_DebugName;
}