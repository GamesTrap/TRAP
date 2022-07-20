#include "TRAPPCH.h"
#include "Layer.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Layer::Layer(std::string debugName)
	: m_DebugName(std::move(debugName))
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnAttach()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnDetach()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnUpdate(const Utils::TimeStep& /*deltaTime*/)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnTick()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnImGuiRender()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Layer::OnEvent(Events::Event& /*event*/)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Layer::GetName() const
{
	return m_DebugName;
}