#include "WindowStateTests.h"

namespace
{
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event)
	{
		TP_TRACE(event);

		const TRAP::Window* const window = TRAP::Application::GetWindow();

		switch(event.GetKey())
		{
		case TRAP::Input::Key::I:
			window->Minimize();
			break;

		case TRAP::Input::Key::M:
			window->Maximize();
			break;

		case TRAP::Input::Key::R:
			window->Restore();
			break;

		case TRAP::Input::Key::Escape:
			TRAP::Application::Shutdown();
			break;

		case TRAP::Input::Key::B:
			window->SetResizable(!window->IsResizable());
			break;

		case TRAP::Input::Key::One:
			window->SetProgress(TRAP::Window::ProgressState::Disabled, 0.0);
			break;
		case TRAP::Input::Key::Two:
			window->SetProgress(TRAP::Window::ProgressState::Normal, 0.25);
			break;
		case TRAP::Input::Key::Three:
			window->SetProgress(TRAP::Window::ProgressState::Normal, 0.50);
			break;
		case TRAP::Input::Key::Four:
			window->SetProgress(TRAP::Window::ProgressState::Normal, 0.75);
			break;
		case TRAP::Input::Key::Five:
			window->SetProgress(TRAP::Window::ProgressState::Normal, 1.0);
			break;
		case TRAP::Input::Key::Six:
			window->SetProgress(TRAP::Window::ProgressState::Indeterminate, 0.0);
			break;
		case TRAP::Input::Key::Seven:
			window->SetProgress(TRAP::Window::ProgressState::Error, 1.0);
			break;
		case TRAP::Input::Key::Eight:
			window->SetProgress(TRAP::Window::ProgressState::Paused, 1.0);
			break;
		case TRAP::Input::Key::Nine:
			window->RequestAttention();
			break;

		default:
			break;
		}

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnWindowResize(const TRAP::Events::WindowResizeEvent& event)
	{
		TP_TRACE(event);

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event)
	{
		TP_TRACE(event);

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnWindowFocus(const TRAP::Events::WindowFocusEvent& event)
	{
		TP_TRACE(event);

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnWindowLostFocus(const TRAP::Events::WindowLostFocusEvent& event)
	{
		TP_TRACE(event);

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnWindowMinimize(const TRAP::Events::WindowMinimizeEvent& event)
	{
		TP_TRACE(event);

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnWindowMaximize(const TRAP::Events::WindowMaximizeEvent& event)
	{
		TP_TRACE(event);

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnWindowRestore(const TRAP::Events::WindowRestoreEvent& event)
	{
		TP_TRACE(event);

		return true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void WindowStateTests::OnImGuiRender()
{
	ImGui::Begin("WindowState", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                     ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Text("Press I to minimize window");
	ImGui::Text("Press M to maximize window");
	ImGui::Text("Press R to restore window");
	ImGui::Text("Press B to enable/disable window resizability");
	ImGui::Separator();
	ImGui::Text("Press 1 to set progress to   0%%");
	ImGui::Text("Press 2 to set progress to  25%%");
	ImGui::Text("Press 3 to set progress to  50%%");
	ImGui::Text("Press 4 to set progress to  75%%");
	ImGui::Text("Press 5 to set progress to 100%%");
	ImGui::Text("Press 6 to set progress to Indeterminate");
	ImGui::Text("Press 7 to set progress to Error");
	ImGui::Text("Press 8 to set progress to Paused");
	ImGui::Text("Press 9 to request user attention");
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

	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(OnKeyPress);
	dispatcher.Dispatch<TRAP::Events::WindowResizeEvent>(OnWindowResize);
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(OnFrameBufferResize);
	dispatcher.Dispatch<TRAP::Events::WindowFocusEvent>(OnWindowFocus);
	dispatcher.Dispatch<TRAP::Events::WindowLostFocusEvent>(OnWindowLostFocus);
	dispatcher.Dispatch<TRAP::Events::WindowMinimizeEvent>(OnWindowMinimize);
	dispatcher.Dispatch<TRAP::Events::WindowMaximizeEvent>(OnWindowMaximize);
	dispatcher.Dispatch<TRAP::Events::WindowRestoreEvent>(OnWindowRestore);
}
