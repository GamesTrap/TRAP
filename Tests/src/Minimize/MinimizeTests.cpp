#include "MinimizeTests.h"

MinimizeTests::MinimizeTests()
	: Layer("Minimize")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void MinimizeTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Minimize");
}

//-------------------------------------------------------------------------------------------------------------------//

void MinimizeTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color);
}

//-------------------------------------------------------------------------------------------------------------------//

void MinimizeTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
	dispatcher.Dispatch<TRAP::Events::WindowResizeEvent>([this](TRAP::Events::WindowResizeEvent& e) {return OnWindowResize(e); });
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>([this](TRAP::Events::FrameBufferResizeEvent& e) {return OnFrameBufferResize(e); });
	dispatcher.Dispatch<TRAP::Events::WindowFocusEvent>([this](TRAP::Events::WindowFocusEvent& e) {return OnWindowFocus(e); });
	dispatcher.Dispatch<TRAP::Events::WindowLostFocusEvent>([this](TRAP::Events::WindowLostFocusEvent& e) {return OnWindowLostFocus(e); });
	dispatcher.Dispatch<TRAP::Events::WindowMinimizeEvent>([this](TRAP::Events::WindowMinimizeEvent& e) {return OnWindowMinimize(e); });
	dispatcher.Dispatch<TRAP::Events::WindowMaximizeEvent>([this](TRAP::Events::WindowMaximizeEvent& e) {return OnWindowMaximize(e); });
	dispatcher.Dispatch<TRAP::Events::WindowRestoreEvent>([this](TRAP::Events::WindowRestoreEvent& e) {return OnWindowRestore(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	TP_TRACE(event.ToString());

	if (event.GetRepeatCount() > 0)
		return true;

	switch(event.GetKey())
	{
	case TRAP::Input::Key::I:
		TRAP::Application::GetWindow()->Minimize();
		break;

	case TRAP::Input::Key::M:
		TRAP::Application::GetWindow()->Maximize();
		break;

	case TRAP::Input::Key::R:
		TRAP::Application::GetWindow()->Restore();
		break;

	case TRAP::Input::Key::Escape:
		TRAP::Application::Shutdown();
		break;
		
	case TRAP::Input::Key::B:
		TRAP::Application::GetWindow()->SetResizable(!TRAP::Application::GetWindow()->IsResizable());
		break;
		
	default:
		break;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowResize(const TRAP::Events::WindowResizeEvent& event)
{
	TP_TRACE(event.ToString());
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event)
{
	TP_TRACE(event.ToString());
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowFocus(const TRAP::Events::WindowFocusEvent& event)
{
	TP_TRACE(event.ToString());
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowLostFocus(const TRAP::Events::WindowLostFocusEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowMinimize(const TRAP::Events::WindowMinimizeEvent& event)
{
	TP_TRACE(event.ToString());
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowMaximize(const TRAP::Events::WindowMaximizeEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowRestore(const TRAP::Events::WindowRestoreEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}