#include "EventTests.h"

EventTests::EventTests()
	: Layer("Events")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void EventTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Events");
	TP_TRACE(TRAP::Input::GetKeyboardLayoutName());
}

//-------------------------------------------------------------------------------------------------------------------//

void EventTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color);
}

//-------------------------------------------------------------------------------------------------------------------//

void EventTests::OnEvent(TRAP::Events::Event& event)
{
	TP_TRACE(event.ToString());
}