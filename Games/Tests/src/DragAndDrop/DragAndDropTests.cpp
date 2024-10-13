#include "DragAndDropTests.h"

void DragAndDropTests::OnImGuiRender()
{
	ImGui::Begin("Drag and Drop", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                       ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Text("Press D to enable/disable Drag and Drop");
	ImGui::Text("%s", fmt::format("Drag and Drop: {}", m_dragAndDrop ? "Enabled" : "Disabled").c_str());
	ImGui::Separator();
	ImGui::Text("Data: ");
	if(m_dropData.empty())
		ImGui::Text("No data");
	else
	{
		for (const std::string& str : m_dropData)
			ImGui::Text("%s", str.c_str());
	}
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void DragAndDropTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetDragAndDrop(true);
}

//-------------------------------------------------------------------------------------------------------------------//

bool DragAndDropTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	if(event.GetKey() == TRAP::Input::Key::Escape)
	{
		TRAP::Application::Shutdown();
		return true;
	}

	if(event.GetKey() == TRAP::Input::Key::D)
	{
		m_dragAndDrop = !m_dragAndDrop;
		TRAP::Application::GetWindow()->SetDragAndDrop(m_dragAndDrop);
		return true;
	}

	return false;
}
