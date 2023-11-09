#include "EventTests.h"

void EventTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Events");
}

//-------------------------------------------------------------------------------------------------------------------//

void EventTests::OnEvent(TRAP::Events::Event& event)
{
	TP_TRACE(event);
}