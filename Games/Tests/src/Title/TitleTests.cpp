#include "TitleTests.h"

void TitleTests::OnImGuiRender()
{
	ImGui::Begin("Title", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                               ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void TitleTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("English 日本語 русский язык 官話");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TitleTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	if (event.GetKey() == TRAP::Input::Key::Escape)
	{
		TRAP::Application::Shutdown();
		return true;
	}

	return true;
}
