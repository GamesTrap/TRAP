#include "ClipboardTests.h"

void ClipboardTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Clipboard");
}

//-------------------------------------------------------------------------------------------------------------------//

void ClipboardTests::OnImGuiRender()
{
	ImGui::Begin("Clipboard", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                   ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Press ESC to close");
	ImGui::Text("Press CTRL+C to copy example text from engine");
	ImGui::Text("Press CTRL+V to paste clipboard contents to the engine");
	ImGui::Separator();
	if (!m_clipboard.empty())
		ImGui::Text("Pasted from Clipboard: %s", m_clipboard.c_str());
	else
		ImGui::Text("Nothing pasted yet");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

bool ClipboardTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	switch(event.GetKey())
	{
	case TRAP::Input::Key::Escape:
		TRAP::Application::Shutdown();
		break;

	case TRAP::Input::Key::V:
	{
		if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Control) ||
			TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Control))
		{
			m_clipboard = TRAP::Input::GetClipboard();

			if (!m_clipboard.empty())
				TP_INFO("[Clipboard] Clipboard contains: \"", m_clipboard, "\"");
			else
				TP_INFO("[Clipboard] Clipboard does not contain a string!");
		}

		break;
	}

	case TRAP::Input::Key::C:
	{
		if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Control) ||
			TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Control))
		{
			const std::string clipboard = "Hello TRAP™!";
			TRAP::Input::SetClipboard(clipboard);
			TP_INFO("[Clipboard] Setting clipboard to \"", clipboard, "\"!");
		}

		break;
	}

	default:
		break;
	}

	return true;
}
