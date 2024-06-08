#include "OpacityTests.h"

namespace
{
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event)
	{
		if(event.GetKey() == TRAP::Input::Key::Escape)
		{
			TRAP::Application::Shutdown();
			return true;
		}

		return true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void OpacityTests::OnImGuiRender()
{
	ImGui::Begin("Opacity", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                 ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	if(ImGui::SliderFloat("Opacity", &m_opacity, 0.0f, 1.0f))
		TRAP::Application::GetWindow()->SetOpacity(m_opacity);
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void OpacityTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Opacity");
}

//-------------------------------------------------------------------------------------------------------------------//

void OpacityTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(OnKeyPress);
}
