#include "DragAndDropTests.h"

void DragAndDropTests::OnImGuiRender()
{
	ImGui::Begin("Drag and Drop", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                       ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::TextFmt("Press ESC to close");
	ImGui::TextFmt("Press D to enable/disable Drag and Drop");
	ImGui::TextFmt("Drag and Drop: {}", m_dragAndDrop ? "Enabled" : "Disabled");
	ImGui::Separator();
	ImGui::TextFmt("Data: ");
	if(m_dropData.empty())
		ImGui::TextFmt("No data");
	else
	{
		for (const std::string& str : m_dropData)
			ImGui::TextFmt("{}", str);
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
