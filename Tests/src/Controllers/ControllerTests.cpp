#include "ControllerTests.h"

std::vector<TRAP::Input::Controller> ControllerTests::s_controllers{};
bool ControllerTests::s_dpadButtons = false;

//-------------------------------------------------------------------------------------------------------------------//

ControllerTests::ControllerTests()
	: Layer("Controllers")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void ControllerTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Controllers");

	for(uint32_t controller = static_cast<uint32_t>(TRAP::Input::Controller::One); controller <= static_cast<uint32_t>(TRAP::Input::Controller::Sixteen); controller++)
		if (TRAP::Input::IsControllerConnected(static_cast<TRAP::Input::Controller>(controller)))
			s_controllers.push_back(static_cast<TRAP::Input::Controller>(controller));
}

//-------------------------------------------------------------------------------------------------------------------//

void ControllerTests::OnImGuiRender()
{
	ImGui::Begin("Controllers", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNavInputs);
	if (!s_controllers.empty())
	{
		ImGui::Checkbox("DPad Buttons", &s_dpadButtons);
		for (const TRAP::Input::Controller& controller : s_controllers)
			if (ImGui::Button((std::to_string(static_cast<uint32_t>(controller) + 1) + ". " + TRAP::Input::GetControllerName(controller)).c_str()))
				ImGui::SetWindowFocus((std::to_string(static_cast<uint32_t>(controller) + 1) + ". " + TRAP::Input::GetControllerName(controller)).c_str());
	}
	else
		ImGui::Text("No Controllers Connected!");
	ImGui::End();

	for(const TRAP::Input::Controller& controller : s_controllers)
	{
		std::vector<float> axes = TRAP::Input::GetAllControllerAxes(controller);
		std::vector<bool> buttons = TRAP::Input::GetAllControllerButtons(controller);
		std::vector<TRAP::Input::ControllerDPad> dpad = TRAP::Input::GetAllControllerDPads(controller);
		
		ImGui::Begin((std::to_string(static_cast<uint32_t>(controller) + 1) + ". " + TRAP::Input::GetControllerName(controller)).c_str(), nullptr, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs);
		ImGui::Text("Hardware GUID: %s", TRAP::Input::GetControllerGUID(controller).c_str());
		ImGui::NewLine();
		ImGui::Text("Controller State");
		for(const float axis : axes)
		{
			float axisCpy = axis;
			ImGui::SliderFloat("", &axisCpy, -1.0f, 1.0f);
		}
		for(uint32_t i = 0; i < (s_dpadButtons ? buttons.size() : buttons.size() - dpad.size() * 4); i++)
		{
			bool cpy = buttons[i];
			ImGui::Checkbox(std::to_string(i + 1).c_str(), &cpy);
			if (i % 2 == 0)
				ImGui::SameLine(ImGui::GetContentRegionAvailWidth() / 2.0f);
		}

		for(uint32_t i = 0; i < dpad.size(); i++)
		{
			std::string dpadText = "DPad " + std::to_string(i) + ": ";
			dpadText += GetDPadDirection(dpad[i]);
			bool dpadPressed = static_cast<bool>(dpad[i]);
			ImGui::Checkbox(dpadText.c_str(), &dpadPressed);
			if (i % 2 == 0)
				ImGui::SameLine(ImGui::GetContentRegionAvailWidth() / 2.0f);
		}
		
		if (!buttons.empty())
			ImGui::NewLine();
		
		if (TRAP::Input::IsControllerGamepad(controller))
		{
			static constexpr std::array<const char*, static_cast<uint32_t>(TRAP::Input::ControllerButton::DPad_Left) - 4> buttonNames =
			{
				"A", "B", "X", "Y",
				"LB", "RB",
				"Back", "Start",
				"LT", "RT"
			};
			static constexpr std::array<const char*, static_cast<uint32_t>(TRAP::Input::ControllerAxis::Right_Trigger) + 1> axesNames =
			{
				"Left X", "Left Y",
				"Right X", "Right Y",
				"Left Trigger", "Right Trigger"
			};

			ImGui::Text(("Gamepad State: " + TRAP::Input::GetControllerName(controller)).c_str());

			for (uint32_t i = 0; i < axes.size(); i++)
			{
				float axisCpy = axes[i];
				ImGui::SliderFloat(axesNames[i], &axisCpy, -1.0f, 1.0f);
			}

			for (uint32_t i = 0; i < buttons.size() - dpad.size() * 4; i++)
			{
				bool cpy = buttons[i];
				ImGui::Checkbox(buttonNames[i], &cpy);
				if (i % 2 == 0)
					ImGui::SameLine(ImGui::GetContentRegionAvailWidth() / 2.0f);
			}

			if(!dpad.empty())
			{
				std::string dpadText = "DPad: ";
				dpadText += GetDPadDirection(dpad[0]);
				bool dpadPressed = static_cast<bool>(dpad[0]);
				ImGui::Checkbox(dpadText.c_str(), &dpadPressed);
			}
			
			ImGui::NewLine();
			static bool vibration = false;
			ImGui::Checkbox("Vibration Test", &vibration);
			if (vibration)
				TRAP::Input::SetControllerVibration(controller, 1.0f, 1.0f);
			else
				TRAP::Input::SetControllerVibration(controller, 0.0f, 0.0f);
		}
		else
			ImGui::Text("Controller has no Gamepad mapping!");
		ImGui::End();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void ControllerTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color);

}

//-------------------------------------------------------------------------------------------------------------------//

void ControllerTests::OnEvent(TRAP::Event& event)
{
	TRAP::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::ControllerConnectEvent>([this](TRAP::ControllerConnectEvent& e) { return OnControllerConnect(e); });
	dispatcher.Dispatch<TRAP::ControllerDisconnectEvent>([this](TRAP::ControllerDisconnectEvent& e) { return OnControllerDisconnect(e); });
	dispatcher.Dispatch<TRAP::WindowDropEvent>([this](TRAP::WindowDropEvent& e) { return OnWindowDrop(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool ControllerTests::OnControllerConnect(const TRAP::ControllerConnectEvent& event)
{
	s_controllers.push_back(event.GetController());
	
	if (!TRAP::Application::GetWindow()->IsFocused())
		TRAP::Application::GetWindow()->RequestAttention();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ControllerTests::OnControllerDisconnect(const TRAP::ControllerDisconnectEvent& event)
{
	for(uint32_t i = 0; i < s_controllers.size(); i++)
	{
		if(s_controllers[i] == event.GetController())
		{
			s_controllers.erase(s_controllers.begin() + i);
			break;
		}
	}
	
	if (!TRAP::Application::GetWindow()->IsFocused())
		TRAP::Application::GetWindow()->RequestAttention();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ControllerTests::OnWindowDrop(const TRAP::WindowDropEvent& event)
{
	std::vector<std::string> paths = event.GetPaths();
	
	for (const std::string& path : paths)
	{
		std::string data = TRAP::VFS::ReadTextFile(path);
		if(!data.empty())
			TRAP::Input::UpdateControllerMappings(data);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string ControllerTests::GetDPadDirection(const TRAP::Input::ControllerDPad& dpad)
{
	switch (dpad)
	{
	case TRAP::Input::ControllerDPad::Centered:
		return "Centered";

	case TRAP::Input::ControllerDPad::Up:
		return "Up";

	case TRAP::Input::ControllerDPad::Right:
		return "Right";

	case TRAP::Input::ControllerDPad::Down:
		return "Down";

	case TRAP::Input::ControllerDPad::Left:
		return "Left";

	case TRAP::Input::ControllerDPad::Right_Up:
		return "Right Up";

	case TRAP::Input::ControllerDPad::Right_Down:
		return "Right Down";

	case TRAP::Input::ControllerDPad::Left_Up:
		return "Left Up";

	case TRAP::Input::ControllerDPad::Left_Down:
		return "Left Down";

	default:
		return "";
	}
}