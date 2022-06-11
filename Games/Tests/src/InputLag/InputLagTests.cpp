#include "InputLagTests.h"

InputLagTests::InputLagTests()
	: Layer("InputLag"), m_cursorMethod(CursorMethod::SyncQuery),
	  m_vsync(TRAP::Application::GetConfig().Get<bool>("VSync")),
	  m_cursorNew(), m_cursorPos(), m_cursorVelocity(), m_showForecasts(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("InputLag");
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnImGuiRender()
{
	ImGui::Begin("InputLag", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                  ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Press ESC to close");
	ImGui::Text("VSync (V): %s", m_vsync ? "Enabled" : "Disabled");
	ImGui::Separator();
	//Draw indicators on mouse pos
	for(int32_t lead = m_showForecasts ? 3 : 0; lead >= 0; --lead)
		DrawMarker(lead, TRAP::Math::Vec2(m_cursorPos + m_cursorVelocity * static_cast<float>(lead)));
	//Draw instructions
	ImGui::Text("Move mouse uniformly and check marker under cursor:");
	for(uint32_t lead = 0; lead <= 3; ++lead)
	{
		if(lead == 0)
			ImGui::Text("Red circle - Current cursor position (no input lag)");
		else if(lead == 1)
			ImGui::Text("Yellow circle - %d-frame forecast (input lag is %d frame)", lead, lead);
		else if(lead == 2)
			ImGui::Text("Green circle  - %d-frame forecast (input lag is %d frame)", lead, lead);
		else if(lead == 3)
			ImGui::Text("Blue circle   - %d-frame forecast (input lag is %d frame)", lead, lead);
	}
	ImGui::Text("Current input method: %s", m_cursorMethod == CursorMethod::SyncQuery ? "Sync query" : "Latest input message");
	ImGui::Separator();
	ImGui::Checkbox("Show forecasts", &m_showForecasts);
	if(ImGui::RadioButton("TRAP::Input::GetMousePosition (sync query)", m_cursorMethod == CursorMethod::SyncQuery))
		m_cursorMethod = CursorMethod::SyncQuery;
	if(ImGui::RadioButton("TRAP::Events::MouseMoveEvent (latest input message)", m_cursorMethod == CursorMethod::InputMessage))
		m_cursorMethod = CursorMethod::InputMessage;
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	SampleInput();
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
	dispatcher.Dispatch<TRAP::Events::MouseMoveEvent>([this](TRAP::Events::MouseMoveEvent& e)
	{
		return OnMouseMove(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool InputLagTests::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
	switch(event.GetKey())
	{
	case TRAP::Input::Key::Escape:
		TRAP::Application::Shutdown();
		break;

	case TRAP::Input::Key::V:
	{
		m_vsync = !m_vsync;
		TP_TRACE("[InputLag] VSync: ", m_vsync ? "On" : "Off");
		event.GetWindow()->SetVSync(m_vsync);
		break;
	}

	default:
		break;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool InputLagTests::OnMouseMove(TRAP::Events::MouseMoveEvent& event)
{
	if(m_cursorMethod == CursorMethod::InputMessage)
	{
		m_cursorNew = event.GetPosition();
		m_cursorNew += event.GetWindow()->GetPosition();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::SampleInput()
{
	static constexpr float a = 0.25f; //Exponential smoothing factor

	if(m_cursorMethod == CursorMethod::SyncQuery)
	{
		m_cursorNew = TRAP::Input::GetMousePosition();
		m_cursorNew += TRAP::Application::GetWindow()->GetPosition();
	}


	m_cursorVelocity = (m_cursorNew - m_cursorPos) * a + m_cursorVelocity * (1 - a);
	m_cursorPos = m_cursorNew;
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::DrawMarker(const int32_t lead, TRAP::Math::Vec2 pos)
{
	static const std::array<ImColor, 4> colors{ImColor(255, 0, 0), ImColor(255, 255, 0), ImColor(0, 255, 0), ImColor(0, 96, 255)};

	auto* drawList = ImGui::GetBackgroundDrawList();
	drawList->AddCircleFilled(ImVec2(pos.x, pos.y), 10, colors[lead], 0);
}