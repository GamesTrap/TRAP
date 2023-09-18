#include "DragAndDropTests.h"

void DragAndDropTests::OnImGuiRender()
{
	ImGui::Begin("Drag and Drop", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                       ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Text("Press D to enable/disable Drag and Drop");
	if(m_dragAndDrop)
		ImGui::Text("Drag and Drop: Enabled");
	else
		ImGui::Text("Drag and Drop: Disabled");
	ImGui::Separator();
	ImGui::Text("Data: ");
	if(m_dropData.empty())
		ImGui::Text("No data");
	else
	{
		for (const std::string_view str : m_dropData)
			ImGui::Text("%s", str.data());
	}
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void DragAndDropTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetDragAndDrop(true);
}

//-------------------------------------------------------------------------------------------------------------------//

void DragAndDropTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(this, &DragAndDropTests::OnKeyPress);
	dispatcher.Dispatch<TRAP::Events::WindowDropEvent>(this, &DragAndDropTests::OnDrop);
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

//-------------------------------------------------------------------------------------------------------------------//

bool DragAndDropTests::OnDrop(const TRAP::Events::WindowDropEvent& event)
{
	m_dropData = event.GetPaths();

	return true;
}