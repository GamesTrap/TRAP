#include "TitleTests.h"

TitleTests::TitleTests()
	: Layer("Title")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TitleTests::OnImGuiRender()
{
	ImGui::Begin("CursorTest", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
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

void TitleTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool TitleTests::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
	if (event.GetKey() == TRAP::Input::Key::Escape)
	{
		TRAP::Application::Shutdown();
		return true;
	}

	return true;
}