#include "Sandbox2D.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"),
	m_cameraController(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight())/*, false, true, TRAP::Input::Controller::One*/),
	m_frameTimeHistory()
{
	//TODO
	//If controller connects switch to it
	//If controller disconnect switch to mouse/keyboard
	
	//TODO ControllerAPI validation tests
}

//-------------------------------------------------------------------------------------------------------------------//

std::string ControllerAPIToString(const TRAP::Input::ControllerAPI controllerAPI)
{
	switch(controllerAPI)
	{
	case TRAP::Input::ControllerAPI::XInput:
		return "XInput";
		
	case TRAP::Input::ControllerAPI::DirectInput:
		return "DirectInput";

	case TRAP::Input::ControllerAPI::Linux:
		return "Linux";
		
	default:
		return "Unknown";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string DPadToString(const TRAP::Input::ControllerDPad dpad)
{
	switch(dpad)
	{
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

	case TRAP::Input::ControllerDPad::Left_Down:
		return "Left Down";

	case TRAP::Input::ControllerDPad::Left_Up:
		return "Left Up";
		
	default:
		return "Centered";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnImGuiRender()
{
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGui::Text("Performance");
	ImGui::Separator();
	ImGui::Text("DrawCalls: %u", TRAP::Graphics::Renderer::GetDrawCalls());
	ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
	ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
	ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
	ImGui::End();

	if(TRAP::Input::IsControllerConnected(TRAP::Input::Controller::One))
	{
		if(TRAP::Input::IsControllerGamepad(TRAP::Input::Controller::One))
		{
			ImGui::SetNextWindowBgAlpha(0.3f);
			ImGui::Begin("Controller 1:");
			ImGui::Separator();
			ImGui::Text("Infos");
			ImGui::Text("API: %s", ControllerAPIToString(TRAP::Input::GetControllerAPI()).c_str());
			ImGui::Text("Name: %s", TRAP::Input::GetControllerName(TRAP::Input::Controller::One).c_str());
			ImGui::Text("Status: %s", TRAP::Input::IsControllerConnected(TRAP::Input::Controller::One) ? "Connected" : "Disconnected");
			ImGui::Text("Gamepad: %s", TRAP::Input::IsControllerGamepad(TRAP::Input::Controller::One) ? "True" : "False");
			ImGui::Separator();
			ImGui::Text("Axes");
			ImGui::Text("Left X: %f", TRAP::Input::GetControllerAxis(TRAP::Input::Controller::One, TRAP::Input::ControllerAxis::Left_X));
			ImGui::Text("Left Y: %f", TRAP::Input::GetControllerAxis(TRAP::Input::Controller::One, TRAP::Input::ControllerAxis::Left_Y));
			ImGui::Text("Right X: %f", TRAP::Input::GetControllerAxis(TRAP::Input::Controller::One, TRAP::Input::ControllerAxis::Right_X));
			ImGui::Text("Right Y: %f", TRAP::Input::GetControllerAxis(TRAP::Input::Controller::One, TRAP::Input::ControllerAxis::Right_Y));
			ImGui::Text("Left Trigger: %f", TRAP::Input::GetControllerAxis(TRAP::Input::Controller::One, TRAP::Input::ControllerAxis::Left_Trigger));
			ImGui::Text("Right Trigger: %f", TRAP::Input::GetControllerAxis(TRAP::Input::Controller::One, TRAP::Input::ControllerAxis::Right_Trigger));
			ImGui::Separator();
			ImGui::Text("DPad");
			ImGui::Text("DPad: %s", DPadToString(TRAP::Input::GetControllerDPad(TRAP::Input::Controller::One, 0)).c_str());
			ImGui::Separator();
			ImGui::Text("Buttons");
			ImGui::Text("A / Cross: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::A) ? "True" : "False");
			ImGui::Text("B / Circle: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::B) ? "True" : "False");
			ImGui::Text("X / Square: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::X) ? "True" : "False");
			ImGui::Text("Y / Triangle: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::Y) ? "True" : "False");
			ImGui::Text("Left Bumper: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::Left_Bumper) ? "True" : "False");
			ImGui::Text("Right Bumper: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::Right_Bumper) ? "True" : "False");
			ImGui::Text("Back: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::Back) ? "True" : "False");
			ImGui::Text("Start: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::Start) ? "True" : "False");
			ImGui::Text("Guide: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::Guide) ? "True" : "False");
			ImGui::Text("Left Thumb: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::Left_Thumb) ? "True" : "False");
			ImGui::Text("Right Thumb: %s", TRAP::Input::IsGamepadButtonPressed(TRAP::Input::Controller::One, TRAP::Input::ControllerButton::Right_Thumb) ? "True" : "False");
			ImGui::End();
		}
		else
		{
			ImGui::SetNextWindowBgAlpha(0.3f);
			ImGui::Begin("Controller 1:");
			ImGui::Separator();
			ImGui::Text("Infos");
			ImGui::Text("API: %s", ControllerAPIToString(TRAP::Input::GetControllerAPI()).c_str());
			ImGui::Text("Name: %s", TRAP::Input::GetControllerName(TRAP::Input::Controller::One).c_str());
			ImGui::Text("Status: %s", TRAP::Input::IsControllerConnected(TRAP::Input::Controller::One) ? "Connected" : "Disconnected");
			ImGui::Text("Gamepad: %s", TRAP::Input::IsControllerGamepad(TRAP::Input::Controller::One) ? "True" : "False");
			ImGui::Separator();
			ImGui::Text("Axes");
			for(uint32_t i = 0; i < TRAP::Input::GetAllControllerAxes(TRAP::Input::Controller::One).size(); i++)
				ImGui::Text("%i: %f", i, TRAP::Input::GetAllControllerAxes(TRAP::Input::Controller::One)[i]);
			ImGui::Separator();
			ImGui::Text("DPad");
			for(uint32_t i = 0; i < TRAP::Input::GetAllControllerDPads(TRAP::Input::Controller::One).size(); i++)
				ImGui::Text("%i: %s", i, DPadToString(TRAP::Input::GetAllControllerDPads(TRAP::Input::Controller::One)[i]).c_str());
			ImGui::Separator();
			ImGui::Text("Buttons");
			for(uint32_t i = 0; i < TRAP::Input::GetAllControllerButtons(TRAP::Input::Controller::One).size(); i++)
				ImGui::Text("%i: %s", i, TRAP::Input::GetAllControllerButtons(TRAP::Input::Controller::One)[i] ? "True" : "False");
			ImGui::End();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnAttach()
{
	TRAP::Application::SetHotShaderReloading(true);
	TRAP::Application::SetHotTextureReloading(true);	
	TRAP::Application::GetWindow()->SetTitle("Sandbox2D");
	
	//Mount & Load Textures
	TRAP::VFS::MountTextures("Assets/Textures");
	TRAP::Graphics::TextureManager::Load("TRAP", "/Textures/TRAPWhiteLogo2048x2048.png");
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnDetach()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{	
	//Update
	m_cameraController.OnUpdate(deltaTime);

	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);

	TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());	
	TRAP::Graphics::Renderer2D::DrawQuad(TRAP::Math::Vec2(-1.0f, 0.0f), TRAP::Math::Vec2(0.8f, 0.8f), TRAP::Math::Vec4(0.8f, 0.2f, 0.3f, 1.0f));
	TRAP::Graphics::Renderer2D::DrawQuad(TRAP::Math::Vec2(0.5f, -0.5f), TRAP::Math::Vec2(0.5f, 0.75f), TRAP::Math::Vec4(0.2f, 0.3f, 0.8f, 1.0f));
	TRAP::Graphics::Renderer2D::DrawQuad(TRAP::Math::Vec2(0.2f, 0.5f), TRAP::Math::Vec2(0.5f, 0.5f), TRAP::Math::Vec4(0.2f, 0.3f, 0.8f, 1.0f), TRAP::Graphics::TextureManager::Get2D("TRAP"));
	TRAP::Graphics::Renderer2D::EndScene();
	
	//FPS & FrameTime
	if (m_updateFPSTimer.Elapsed() >= 0.025f)
	{
		m_updateFPSTimer.Reset();
		static int frameTimeIndex = 0;
		if (frameTimeIndex < static_cast<int>(m_frameTimeHistory.size() - 1))
		{
			m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::Renderer::GetFrameTime();
			frameTimeIndex++;
		}
		else
		{
			std::move(m_frameTimeHistory.begin() + 1, m_frameTimeHistory.end(), m_frameTimeHistory.begin());
			m_frameTimeHistory[m_frameTimeHistory.size() - 1] = TRAP::Graphics::Renderer::GetFrameTime();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnEvent(TRAP::Event& event)
{
	m_cameraController.OnEvent(event);
}