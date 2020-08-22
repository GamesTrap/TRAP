#include "DragAndDropTests.h"

DragAndDropTests::DragAndDropTests()
	: Layer("DragAndDrop"), m_dragAndDrop(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void DragAndDropTests::OnImGuiRender()
{
	ImGui::Begin("Drag And Drop", nullptr);
	if(m_dragAndDrop)
		ImGui::Text("Drag And Drop: Enabled");
	else
		ImGui::Text("Drag And Drop: Disabled");
	ImGui::Separator();
	ImGui::Text("Data: ");
	for (const std::string& str : m_dropData)
		ImGui::Text("%s", str.c_str());
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void DragAndDropTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);
}

//-------------------------------------------------------------------------------------------------------------------//

void DragAndDropTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
	dispatcher.Dispatch<TRAP::Events::WindowDropEvent>([this](TRAP::Events::WindowDropEvent& e) { return OnDrop(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool DragAndDropTests::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
	if(event.GetKey() == TRAP::Input::Key::D)
	{
		m_dragAndDrop = !m_dragAndDrop;
		TRAP::Application::GetWindow()->SetDragAndDrop(m_dragAndDrop);
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool DragAndDropTests::OnDrop(TRAP::Events::WindowDropEvent& event)
{
	m_dropData = event.GetPaths();
	
	return true;
}