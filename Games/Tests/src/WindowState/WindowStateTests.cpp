#include "WindowStateTests.h"

WindowStateTests::WindowStateTests()
	: Layer("WindowState")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void WindowStateTests::OnImGuiRender()
{
	ImGui::Begin("WindowState", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
										 ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGui::Text("Press ESC to close");
	ImGui::Text("Press I to minimize window");
	ImGui::Text("Press M to maximize window");
	ImGui::Text("Press R to restore window");
	ImGui::Text("Press B to enable/disable window resizability");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void WindowStateTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("WindowState");
}

//-------------------------------------------------------------------------------------------------------------------//

void WindowStateTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
	dispatcher.Dispatch<TRAP::Events::WindowResizeEvent>([this](TRAP::Events::WindowResizeEvent& e)
	{
		return OnWindowResize(e);
	});
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>([this](TRAP::Events::FrameBufferResizeEvent& e)
	{
		return OnFrameBufferResize(e);
	});
	dispatcher.Dispatch<TRAP::Events::WindowFocusEvent>([this](TRAP::Events::WindowFocusEvent& e)
	{
		return OnWindowFocus(e);
	});
	dispatcher.Dispatch<TRAP::Events::WindowLostFocusEvent>([this](TRAP::Events::WindowLostFocusEvent& e)
	{
		return OnWindowLostFocus(e);
	});
	dispatcher.Dispatch<TRAP::Events::WindowMinimizeEvent>([this](TRAP::Events::WindowMinimizeEvent& e)
	{
		return OnWindowMinimize(e);
	});
	dispatcher.Dispatch<TRAP::Events::WindowMaximizeEvent>([this](TRAP::Events::WindowMaximizeEvent& e)
	{
		return OnWindowMaximize(e);
	});
	dispatcher.Dispatch<TRAP::Events::WindowRestoreEvent>([this](TRAP::Events::WindowRestoreEvent& e)
	{
		return OnWindowRestore(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool WindowStateTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
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

bool WindowStateTests::OnWindowResize(const TRAP::Events::WindowResizeEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool WindowStateTests::OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool WindowStateTests::OnWindowFocus(const TRAP::Events::WindowFocusEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool WindowStateTests::OnWindowLostFocus(const TRAP::Events::WindowLostFocusEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool WindowStateTests::OnWindowMinimize(const TRAP::Events::WindowMinimizeEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool WindowStateTests::OnWindowMaximize(const TRAP::Events::WindowMaximizeEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool WindowStateTests::OnWindowRestore(const TRAP::Events::WindowRestoreEvent& event)
{
	TP_TRACE(event.ToString());

	return true;
}