#include "WindowFeaturesTests.h"

void WindowFeaturesTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Window Features");

	m_lastWindowPos = TRAP::Application::GetWindow()->GetPosition();
	m_lastWindowSize = TRAP::Application::GetWindow()->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

void WindowFeaturesTests::OnImGuiRender()
{
	ImGui::Begin("WindowFeatures", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                        ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Press ESC to close");
	ImGui::Separator();

	if(ImGui::BeginCombo("Display Mode", fmt::format("{}", TRAP::Application::GetWindow()->GetDisplayMode()).c_str()))
	{
		if(ImGui::Selectable(fmt::format("{}", TRAP::Window::DisplayMode::Windowed).c_str(), TRAP::Application::GetWindow()->GetDisplayMode() == TRAP::Window::DisplayMode::Windowed))
			TRAP::Application::GetWindow()->SetWindowed();
		if(ImGui::Selectable(fmt::format("{}", TRAP::Window::DisplayMode::Borderless).c_str(), TRAP::Application::GetWindow()->GetDisplayMode() == TRAP::Window::DisplayMode::Borderless))
			TRAP::Application::GetWindow()->SetFullscreenBorderless();
		if(ImGui::Selectable(fmt::format("{}", TRAP::Window::DisplayMode::Fullscreen).c_str(), TRAP::Application::GetWindow()->GetDisplayMode() == TRAP::Window::DisplayMode::Fullscreen))
			TRAP::Application::GetWindow()->SetFullscreen();

		ImGui::EndCombo();
	}

	if(ImGui::Button("Maximize"))
		TRAP::Application::GetWindow()->Maximize();
	ImGui::SameLine();
	if(ImGui::Button("Minimize"))
		TRAP::Application::GetWindow()->Minimize();
	ImGui::SameLine();
	if(ImGui::Button("Restore"))
		TRAP::Application::GetWindow()->Restore();
	ImGui::SameLine();
	if(ImGui::Button("Hide (briefly)"))
	{
		const f32 time = TRAP::Application::GetTime().GetSeconds() + 3.0f;

		TRAP::Application::GetWindow()->Hide();

		while(TRAP::Application::GetTime().GetSeconds() < time)
			TRAP::INTERNAL::WindowingAPI::WaitEvents(0.1);

		TRAP::Application::GetWindow()->Show();
	}

	ImGui::Text("Press Enter in a text field to set value");
	//Window Position
	{
		TRAP::Math::Vec2i winPos = TRAP::Application::GetWindow()->GetPosition();
		u32 changed = 0;

		ImGui::Text("Position");
		ImGui::SameLine();
		changed += NumericCast<u32>(ImGui::InputInt("##PosX", &winPos.x(), 0, ImGuiInputTextFlags_EnterReturnsTrue));
		ImGui::SameLine();
		changed += NumericCast<u32>(ImGui::InputInt("##PosY", &winPos.y(), 0, ImGuiInputTextFlags_EnterReturnsTrue));

		if(changed != 0)
		{
			TRAP::Application::GetWindow()->SetPosition(NumericCast<u32>(winPos.x()), NumericCast<u32>(winPos.y()));
			m_lastWindowPos = winPos;
		}
	}
	//Window Size
	{
		TRAP::Math::Vec2ui winSize = TRAP::Application::GetWindow()->GetSize();
		u32 changed = 0;

		ImGui::Text("Size");
		ImGui::SameLine();
		changed += NumericCast<u32>(ImGui::InputScalar("##SizeX", ImGuiDataType_U32, &winSize.x(), nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue));
		ImGui::SameLine();
		changed += NumericCast<u32>(ImGui::InputScalar("##SizeY", ImGuiDataType_U32, &winSize.y(), nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue));

		if(changed != 0)
		{
			TRAP::Application::GetWindow()->SetWindowed(winSize.x(), winSize.y());
			m_lastWindowSize = winSize;
		}
	}
	//Minimum Size
	{
		u32 changed = 0;
		bool updateSizeLimit = false;
		if(ImGui::Checkbox("Minimum Size", &m_limitMinSize))
			updateSizeLimit = true;

		ImGui::SameLine();
		changed += NumericCast<u32>(ImGui::InputScalar("##MinSizeX", ImGuiDataType_U32, &m_minWindowSize.x(), nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue));
		ImGui::SameLine();
		changed += NumericCast<u32>(ImGui::InputScalar("##MinSizeY", ImGuiDataType_U32, &m_minWindowSize.y(), nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue));

		if(changed != 0)
			updateSizeLimit = true;

		if(updateSizeLimit)
		{
			if(m_limitMinSize)
				TRAP::Application::GetWindow()->SetMinimumSize(m_minWindowSize.x(), m_minWindowSize.y());
			else
				TRAP::Application::GetWindow()->SetMinimumSize(0, 0);
		}
	}
	//Maximum Size
	{
		u32 changed = 0;
		bool updateSizeLimit = false;
		if(ImGui::Checkbox("Maximum Size", &m_limitMaxSize))
			updateSizeLimit = true;

		ImGui::SameLine();
		changed += NumericCast<u32>(ImGui::InputScalar("##MaxSizeX", ImGuiDataType_U32, &m_maxWindowSize.x(), nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue));
		ImGui::SameLine();
		changed += NumericCast<u32>(ImGui::InputScalar("##MaxSizeY", ImGuiDataType_U32, &m_maxWindowSize.y(), nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue));

		if(changed != 0)
			updateSizeLimit = true;

		if(updateSizeLimit)
		{
			if(m_limitMaxSize)
				TRAP::Application::GetWindow()->SetMaximumSize(m_maxWindowSize.x(), m_maxWindowSize.y());
			else
				TRAP::Application::GetWindow()->SetMaximumSize(0, 0);
		}
	}

	TRAP::Math::Vec2ui frameBufferSize = TRAP::Application::GetWindow()->GetFrameBufferSize();
	ImGui::Text("Framebuffer Size: %d %d", frameBufferSize.x(), frameBufferSize.y());

	TRAP::Math::Vec2 contentScale = TRAP::Application::GetWindow()->GetMonitor().GetContentScale();
	ImGui::Text("Content Scale: %f %f", contentScale.x(), contentScale.y());

	f32 opacity = TRAP::Application::GetWindow()->GetOpacity().value_or(1.0f);
	if(ImGui::SliderFloat("Opacity", &opacity, 0.001f, 1.0f))
		TRAP::Application::GetWindow()->SetOpacity(opacity);

	bool resizable = TRAP::Application::GetWindow()->IsResizable();
	if(ImGui::Checkbox("Resizable", &resizable))
		TRAP::Application::GetWindow()->SetResizable(resizable);

	ImGui::Separator();
	ImGui::Text("Focused: %s", TRAP::Application::GetWindow()->IsFocused() ? "true" : "false");
	ImGui::Text("Hovered: %s", TRAP::Application::GetWindow()->IsHovered() ? "true" : "false");
	ImGui::Text("Visible: %s", TRAP::Application::GetWindow()->IsVisible() ? "true" : "false");
	ImGui::Text("Minimized: %s", TRAP::Application::GetWindow()->IsMinimized() ? "true" : "false");
	ImGui::Text("Maximized: %s", TRAP::Application::GetWindow()->IsMaximized() ? "true" : "false");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void WindowFeaturesTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void WindowFeaturesTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&WindowFeaturesTests::OnKeyPress, this));
}

//-------------------------------------------------------------------------------------------------------------------//

bool WindowFeaturesTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	switch(event.GetKey())
	{
	case TRAP::Input::Key::Escape:
		TRAP::Application::Shutdown();
		break;

	default:
		break;
	}

	return true;
}