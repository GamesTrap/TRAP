#include "WindowStateTests.h"

void WindowStateTests::OnImGuiRender()
{
	ImGui::Begin("WindowState", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                     ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::TextFmt("Press ESC to close");
	ImGui::TextFmt("Press I to minimize window");
	ImGui::TextFmt("Press M to maximize window");
	ImGui::TextFmt("Press R to restore window");
	ImGui::TextFmt("Press B to enable/disable window resizability");
	ImGui::Separator();
	ImGui::TextFmt("Press 1 to set progress to   0%");
	ImGui::TextFmt("Press 2 to set progress to  25%");
	ImGui::TextFmt("Press 3 to set progress to  50%");
	ImGui::TextFmt("Press 4 to set progress to  75%");
	ImGui::TextFmt("Press 5 to set progress to 100%");
	ImGui::TextFmt("Press 6 to set progress to Indeterminate");
	ImGui::TextFmt("Press 7 to set progress to Error");
	ImGui::TextFmt("Press 8 to set progress to Paused");
	ImGui::TextFmt("Press 9 to request user attention");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void WindowStateTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("WindowState");
}

//-------------------------------------------------------------------------------------------------------------------//

bool WindowStateTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
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
