#include "CursorTests.h"

float CursorTests::Star(const int32_t x, const int32_t y, const float t)
{
	constexpr float c = 64.0f / 2.0f;

	const float i = (0.25f * TRAP::Math::Sin(2.0f * TRAP::Math::PI<float>() * t) + 0.75f);
	const float k = 64.0f * 0.046875f * i;

	const float dist = TRAP::Math::Sqrt((static_cast<float>(x) - c) * (static_cast<float>(x) - c) +
		                                  (static_cast<float>(y) - c) * (static_cast<float>(y) - c));

	const float sAlpha = 1.0f - dist / c;
	const float xAlpha = static_cast<float>(x) == c ? c : k / TRAP::Math::Abs(static_cast<float>(x) - c);
	const float yAlpha = static_cast<float>(y) == c ? c : k / TRAP::Math::Abs(static_cast<float>(y) - c);

	return TRAP::Math::Max(0.0f, TRAP::Math::Min(1.0f, i * sAlpha * 0.2f + sAlpha * xAlpha * yAlpha));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> CursorTests::CreateCursorFrame(const float t)
{
	int32_t i = 0;
	std::vector<uint8_t> buffer(64ull * 64ull * 4ull, 0);

	for(int32_t y = 0; y < 64; y++)
	{
		for(int32_t x = 0; x < 64; x++)
		{
			buffer[i++] = 255;
			buffer[i++] = 255;
			buffer[i++] = 255;
			buffer[i++] = static_cast<uint8_t>(255 * Star(x, y, t));
		}
	}

	return TRAP::Image::LoadFromMemory(64, 64, TRAP::Image::ColorFormat::RGBA, buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

CursorTests::CursorTests()
	: Layer("Cursor"),
	  m_cursorX(0.0f),
	  m_cursorY(0.0f),
	  m_animateCursor(false),
	  m_currentFrame(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void CursorTests::OnImGuiRender()
{
	ImGui::Begin("Cursor", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                    ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Text("Press A to enable/disable Cursor animation");
	ImGui::Text("Press N to set Cursor mode to normal");
	ImGui::Text("Press D to set Cursor mode to disabled");
	ImGui::Text("Press H to set Cursor mode to hidden");
	ImGui::Text("Press C to set Cursor mode to captured");
	ImGui::Text("Press R to set enable/disable Raw Mouse Input (if available)");
	ImGui::Text("Press Up to set mouse position to top left");
	ImGui::Text("Press Down to set mouse position to bottom right");
	ImGui::Text("Press (Shift+)0-9 to cycle through available mouse cursors");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void CursorTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Cursor");

	m_starCursors.reserve(60);
	for(uint32_t i = 0; i < 60; i++)
		m_starCursors.push_back(CreateCursorFrame(static_cast<float>(i) / 60.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

void CursorTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
	if (m_animateCursor)
	{
		const int32_t i = static_cast<int32_t>(TRAP::Application::GetTime() * 30.0f) % 60;
		if (m_currentFrame != m_starCursors[i].get())
		{
			TRAP::Application::GetWindow()->SetCursorIcon(m_starCursors[i].get(), 32, 32);
			m_currentFrame = m_starCursors[i].get();
		}
	}
	else
		m_currentFrame = nullptr;

}

//-------------------------------------------------------------------------------------------------------------------//

void CursorTests::OnEvent(TRAP::Events::Event& event)
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

bool CursorTests::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
	if(event.GetRepeatCount() > 0)
		return true;

	switch (event.GetKey())
	{
	case TRAP::Input::Key::A:
	{
		m_animateCursor = !m_animateCursor;
		if (!m_animateCursor)
			TRAP::Application::GetWindow()->SetCursorType(TRAP::Window::CursorType::Arrow);

		break;
	}

	case TRAP::Input::Key::Escape:
	{
		if (TRAP::Application::GetWindow()->GetCursorMode() != TRAP::Window::CursorMode::Disabled)
			TRAP::Application::Shutdown();

		break;
	}

	case TRAP::Input::Key::N:
	{
		TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Normal);
		m_cursorX = TRAP::Input::GetMouseX();
		m_cursorY = TRAP::Input::GetMouseY();
		TP_INFO("[Cursor] Cursor is normal");

		break;
	}

	case TRAP::Input::Key::D:
	{
		TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Disabled);
		TP_INFO("[Cursor] Cursor is disabled");

		break;
	}

	case TRAP::Input::Key::H:
	{
		TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Hidden);
		TP_INFO("[Cursor] Cursor is hidden");

		break;
	}

	case TRAP::Input::Key::C:
	{
		TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Captured);
		TP_INFO("[Cursor] Cursor is captured");

		break;
	}

	case TRAP::Input::Key::R:
	{
		if (!TRAP::Input::IsRawMouseInputSupported())
			break;

		if (TRAP::Application::GetWindow()->GetRawMouseInput())
		{
			TRAP::Application::GetWindow()->SetRawMouseInput(false);
			TP_INFO("[Cursor] Raw Input disabled");
		}
		else
		{
			TRAP::Application::GetWindow()->SetRawMouseInput(true);
			TP_INFO("[Cursor] Raw Input enabled");
		}

		break;
	}

	case TRAP::Input::Key::P:
	{
		float x = TRAP::Input::GetMouseX();
		float y = TRAP::Input::GetMouseY();

		TP_INFO("[Cursor] Query before set: ", x, " ", y, " (", x - m_cursorX, " ", y - m_cursorY, ")");
		m_cursorX = x;
		m_cursorY = y;

		TRAP::Input::SetMousePosition(m_cursorX, m_cursorY);
		x = TRAP::Input::GetMouseX();
		y = TRAP::Input::GetMouseY();

		TP_INFO("[Cursor] Query after set: ", x, " ", y, " (", x - m_cursorX, " ", y - m_cursorY, ")");
		m_cursorX = x;
		m_cursorY = y;

		break;
	}

	case TRAP::Input::Key::Up:
	{
		TRAP::Input::SetMousePosition(0, 0);
		m_cursorX = TRAP::Input::GetMouseX();
		m_cursorY = TRAP::Input::GetMouseY();

		break;
	}

	case TRAP::Input::Key::Down:
	{
		const uint32_t width = TRAP::Application::GetWindow()->GetWidth();
		const uint32_t height = TRAP::Application::GetWindow()->GetHeight();
		TRAP::Input::SetMousePosition(static_cast<float>(width) - 1.0f, static_cast<float>(height) - 1.0f);
		m_cursorX = TRAP::Input::GetMouseX();
		m_cursorY = TRAP::Input::GetMouseY();

		break;
	}

	case TRAP::Input::Key::Zero:
	{
		TRAP::Application::GetWindow()->SetCursorType(TRAP::Window::CursorType::Arrow);
		break;
	}

	case TRAP::Input::Key::One:
		[[fallthrough]];
	case TRAP::Input::Key::Two:
		[[fallthrough]];
	case TRAP::Input::Key::Three:
		[[fallthrough]];
	case TRAP::Input::Key::Four:
		[[fallthrough]];
	case TRAP::Input::Key::Five:
		[[fallthrough]];
	case TRAP::Input::Key::Six:
		[[fallthrough]];
	case TRAP::Input::Key::Seven:
		[[fallthrough]];
	case TRAP::Input::Key::Eight:
		[[fallthrough]];
	case TRAP::Input::Key::Nine:
	{
		int32_t index = static_cast<int32_t>(event.GetKey()) - static_cast<int32_t>(TRAP::Input::Key::One);
		if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Shift) ||
			TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Shift))
			index += 9;

		if (index <= static_cast<int32_t>(TRAP::Window::CursorType::NotAllowed))
			TRAP::Application::GetWindow()->SetCursorType(static_cast<TRAP::Window::CursorType>(index));

		break;
	}

	default:
		break;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool CursorTests::OnMouseMove(TRAP::Events::MouseMoveEvent& event)
{
	TP_INFO("[Cursor] Position: ", event.GetX(), " ", event.GetY(), " (", event.GetX() - m_cursorX, " ",
	        event.GetY() - m_cursorY, ")");

	m_cursorX = event.GetX();
	m_cursorY = event.GetY();

	return true;
}