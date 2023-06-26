#include "ControllerTests.h"

ControllerTests::ControllerTests()
	: Layer("Controllers")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void ControllerTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Controllers");

	for(uint32_t controller = ToUnderlying(TRAP::Input::Controller::One);
	    controller <= ToUnderlying(TRAP::Input::Controller::Sixteen); controller++)
	{
		if (TRAP::Input::IsControllerConnected(static_cast<TRAP::Input::Controller>(controller)))
			s_controllers.push_back(static_cast<TRAP::Input::Controller>(controller));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void ControllerTests::OnImGuiRender()
{
	ImGui::Begin("Controllers", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
	if (!s_controllers.empty())
	{
		ImGui::Checkbox("DPad Buttons", &s_dpadButtons);
		for (const TRAP::Input::Controller& controller : s_controllers)
		{
			const std::string controllerName = fmt::format("{}. {}", (ToUnderlying(controller) + 1),
			                                               TRAP::Input::GetControllerName(controller));

			if (ImGui::Button(controllerName.c_str()))
				ImGui::SetWindowFocus(controllerName.c_str());
		}
	}
	else
		ImGui::Text("No Controllers Connected!");
	ImGui::End();

	for(const TRAP::Input::Controller& controller : s_controllers)
	{
		const std::vector<float> axes = TRAP::Input::GetAllControllerAxes(controller);
		const std::vector<bool> buttons = TRAP::Input::GetAllControllerButtons(controller);
		const std::vector<TRAP::Input::ControllerDPad> dpads = TRAP::Input::GetAllControllerDPads(controller);

		const std::string controllerName = fmt::format("{}. {}", (ToUnderlying(controller) + 1),
														TRAP::Input::GetControllerName(controller));
		ImGui::Begin(controllerName.c_str());
		ImGui::Text("Hardware GUID: %s", TRAP::Input::GetControllerGUID(controller).c_str());
		ImGui::NewLine();
		ImGui::Text("Controller Battery Status: %s", GetBatteryStatus(controller).c_str());
		ImGui::Text("Controller State");
		for(const float axis : axes)
		{
			float axisCpy = axis;
			ImGui::SliderFloat("##", &axisCpy, -1.0f, 1.0f);
		}
		for(std::size_t i = 0; i < (s_dpadButtons ? buttons.size() : buttons.size() - dpads.size() * 4); i++)
		{
			bool cpy = buttons[i];
			ImGui::Checkbox(std::to_string(i + 1).c_str(), &cpy);
			if (i % 2 == 0)
				ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f);
		}

		for(std::size_t i = 0; i < dpads.size(); i++)
		{
			const std::string dpadText = fmt::format("DPad {}: {}", i, GetDPadDirection(dpads[i]));

			bool dpadPressed = dpads[i] != TRAP::Input::ControllerDPad::Centered;
			ImGui::Checkbox(dpadText.c_str(), &dpadPressed);
			if (i % 2 == 0)
				ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f);
		}

		if (!buttons.empty())
			ImGui::NewLine();

		if (TRAP::Input::IsControllerGamepad(controller))
		{
			ImGui::NewLine();
			ImGui::Text("Mapped State: %s", TRAP::Input::GetControllerName(controller).c_str());

			float leftX = TRAP::Input::GetControllerAxis(controller, TRAP::Input::ControllerAxis::Left_X);
			float leftY = TRAP::Input::GetControllerAxis(controller, TRAP::Input::ControllerAxis::Left_Y);
			float rightX = TRAP::Input::GetControllerAxis(controller, TRAP::Input::ControllerAxis::Right_X);
			float rightY = TRAP::Input::GetControllerAxis(controller, TRAP::Input::ControllerAxis::Right_Y);
			float leftTrigger = TRAP::Input::GetControllerAxis(controller,
															   TRAP::Input::ControllerAxis::Left_Trigger);
			float rightTrigger = TRAP::Input::GetControllerAxis(controller,
			                                                    TRAP::Input::ControllerAxis::Right_Trigger);

			ImGui::SliderFloat("Left X", &leftX, -1.0f, 1.0f);
			ImGui::SliderFloat("Left Y", &leftY, -1.0f, 1.0f);
			ImGui::SliderFloat("Right X", &rightX, -1.0f, 1.0f);
			ImGui::SliderFloat("Right Y", &rightY, -1.0f, 1.0f);
			ImGui::SliderFloat("Left Trigger", &leftTrigger, -1.0f, 1.0f);
			ImGui::SliderFloat("Right Trigger", &rightTrigger, -1.0f, 1.0f);

			bool a = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::A);
			bool b = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::B);
			bool x = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::X);
			bool y = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::Y);
			bool lb = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::Left_Bumper);
			bool rb = TRAP::Input::IsControllerButtonPressed(controller,
															 TRAP::Input::ControllerButton::Right_Bumper);
			bool back = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::Back);
			bool start = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::Start);
			bool lt = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::Left_Thumb);
			bool rt = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::Right_Thumb);
			bool guide = TRAP::Input::IsControllerButtonPressed(controller, TRAP::Input::ControllerButton::Guide);

			ImGui::Checkbox("A", &a);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f);
			ImGui::Checkbox("B", &b);
			ImGui::Checkbox("X", &x);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f);
			ImGui::Checkbox("Y", &y);
			ImGui::Checkbox("Left Bumper", &lb);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f);
			ImGui::Checkbox("Right Bumper", &rb);
			ImGui::Checkbox("Back", &back);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f);
			ImGui::Checkbox("Start", &start);
			ImGui::Checkbox("Left Thumb", &lt);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f);
			ImGui::Checkbox("Right Thumb", &rt);
			ImGui::Checkbox("Guide", &guide);

			TRAP::Input::ControllerDPad dpad = TRAP::Input::GetControllerDPad(controller, 0);
			const std::string dpadText = fmt::format("DPad: {}", GetDPadDirection(dpad));
			bool dpadPressed = dpad != TRAP::Input::ControllerDPad::Centered;
			ImGui::Checkbox(dpadText.c_str(), &dpadPressed);

			ImGui::NewLine();
			constinit static bool vibration = false;
			if(ImGui::Checkbox("Vibration Test", &vibration))
			{
				if (vibration)
					TRAP::Input::SetControllerVibration(controller, 1.0f, 1.0f);
				else
					TRAP::Input::SetControllerVibration(controller, 0.0f, 0.0f);
			}
		}
		else
			ImGui::Text("Controller has no Gamepad mapping!");
		ImGui::End();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void ControllerTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::ControllerConnectEvent>([](TRAP::Events::ControllerConnectEvent& e)
	{
		return OnControllerConnect(e);
	});
	dispatcher.Dispatch<TRAP::Events::ControllerDisconnectEvent>([](TRAP::Events::ControllerDisconnectEvent& e)
	{
		return OnControllerDisconnect(e);
	});
	dispatcher.Dispatch<TRAP::Events::WindowDropEvent>([](TRAP::Events::WindowDropEvent& e)
	{
		return OnWindowDrop(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool ControllerTests::OnControllerConnect(const TRAP::Events::ControllerConnectEvent& event)
{
	s_controllers.push_back(event.GetController());

	if (!TRAP::Application::GetWindow()->IsFocused())
		TRAP::Application::GetWindow()->RequestAttention();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ControllerTests::OnControllerDisconnect(const TRAP::Events::ControllerDisconnectEvent& event)
{
	for(std::size_t i = 0; i < s_controllers.size(); i++)
	{
		if(s_controllers[i] == event.GetController())
		{
			s_controllers.erase(s_controllers.begin() + NumericCast<ptrdiff_t>(i));
			break;
		}
	}

	if (!TRAP::Application::GetWindow()->IsFocused())
		TRAP::Application::GetWindow()->RequestAttention();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ControllerTests::OnWindowDrop(const TRAP::Events::WindowDropEvent& event)
{
	const std::vector<std::string> paths = event.GetPaths();

	for (const std::string_view path : paths)
	{
		const auto data = TRAP::FileSystem::ReadTextFile(path);
		if(data)
			TRAP::Input::UpdateControllerMappings(*data);
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

//-------------------------------------------------------------------------------------------------------------------//

std::string ControllerTests::GetBatteryStatus(const TRAP::Input::Controller controller)
{
	TRAP::Input::ControllerBatteryStatus battery = TRAP::Input::GetControllerBatteryStatus(controller);

	switch(battery)
	{
	case TRAP::Input::ControllerBatteryStatus::Wired:
		return "Wired";

	case TRAP::Input::ControllerBatteryStatus::Empty:
		return "Empty";

	case TRAP::Input::ControllerBatteryStatus::Low:
		return "Low";

	case TRAP::Input::ControllerBatteryStatus::Medium:
		return "Medium";

	case TRAP::Input::ControllerBatteryStatus::Full:
		return "Full";

	default:
		return "";
	}
}