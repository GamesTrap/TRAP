#include "Clipboard.h"

Clipboard::Clipboard()
	: Layer("Clipboard")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void Clipboard::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Clipboard");
	TRAP::Application::GetWindow()->SetVSyncInterval(1);
}

//-------------------------------------------------------------------------------------------------------------------//

void Clipboard::OnImGuiRender()
{
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::Begin("Clipboard", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	if (!m_clipboard.empty())
		ImGui::Text("Clipboard contains: %s", m_clipboard.c_str());
	else
		ImGui::Text("Clipboard is currently empty");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void Clipboard::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color);
}

//-------------------------------------------------------------------------------------------------------------------//

void Clipboard::OnEvent(TRAP::Event& event)
{
	TRAP::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::KeyPressEvent>([this](TRAP::KeyPressEvent& e) { return OnKeyPress(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool Clipboard::OnKeyPress(TRAP::KeyPressEvent& event)
{
	switch(event.GetKeyCode())
	{
	case TRAP::Input::Key::Escape:
		TRAP::Application::Shutdown();
		break;

	case TRAP::Input::Key::V:
	{
		if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Control) ||
			TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Control))
		{
			m_clipboard = TRAP::Input::GetClipboard();

			if (!m_clipboard.empty())
				TP_INFO("[Clipboard] Clipboard contains: \"", m_clipboard, "\"");
			else
				TP_INFO("[Clipboard] Clipboard does not contain a string!");
		}			
		
		break;
	}

	case TRAP::Input::Key::C:
	{
		if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Control) ||
			TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Control))
		{
			const std::string clipboard = "Hello TRAP World!";
			TRAP::Input::SetClipboard(clipboard);
			TP_INFO("[Clipboard] Setting clipboard to \"", clipboard, "\"!");
		}
		
		break;
	}
		
	default:
		break;
	}

	return true;
}