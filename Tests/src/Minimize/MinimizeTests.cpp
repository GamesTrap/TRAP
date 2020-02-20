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

void MinimizeTests::OnEvent(TRAP::Event& event)
{
	TRAP::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<TRAP::KeyPressEvent>([this](TRAP::KeyPressEvent& e) { return OnKeyPress(e); });
	dispatcher.Dispatch<TRAP::WindowResizeEvent>([this](TRAP::WindowResizeEvent& e) {return OnWindowResize(e); });
	dispatcher.Dispatch<TRAP::FrameBufferResizeEvent>([this](TRAP::FrameBufferResizeEvent& e) {return OnFrameBufferResize(e); });
	dispatcher.Dispatch<TRAP::WindowFocusEvent>([this](TRAP::WindowFocusEvent& e) {return OnWindowFocus(e); });
	dispatcher.Dispatch<TRAP::WindowLostFocusEvent>([this](TRAP::WindowLostFocusEvent& e) {return OnWindowLostFocus(e); });
	dispatcher.Dispatch<TRAP::WindowMinimizeEvent>([this](TRAP::WindowMinimizeEvent& e) {return OnWindowMinimize(e); });
	dispatcher.Dispatch<TRAP::WindowMaximizeEvent>([this](TRAP::WindowMaximizeEvent& e) {return OnWindowMaximize(e); });
	dispatcher.Dispatch<TRAP::WindowRestoreEvent>([this](TRAP::WindowRestoreEvent& e) {return OnWindowRestore(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnKeyPress(const TRAP::KeyPressEvent& event)
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

bool MinimizeTests::OnWindowResize(const TRAP::WindowResizeEvent& event)
{
	TP_TRACE(event.ToString());
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnFrameBufferResize(const TRAP::FrameBufferResizeEvent& event)
{
	TP_TRACE(event.ToString());
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowFocus(const TRAP::WindowFocusEvent& event)
{
	TP_TRACE(event.ToString());
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowLostFocus(const TRAP::WindowLostFocusEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowMinimize(const TRAP::WindowMinimizeEvent& event)
{
	TP_TRACE(event.ToString());
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowMaximize(const TRAP::WindowMaximizeEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MinimizeTests::OnWindowRestore(const TRAP::WindowRestoreEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}