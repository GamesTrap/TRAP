#include "CursorTests.h"

f32 CursorTests::Star(const u32 x, const u32 y, const f32 t)
{
	static constexpr f32 c = 64.0f / 2.0f;

	const f32 i = (0.25f * TRAP::Math::Sin(2.0f * TRAP::Math::PI<f32>() * t) + 0.75f);
	const f32 k = 64.0f * 0.046875f * i;

	const f32 dist = TRAP::Math::Sqrt((NumericCast<f32>(x) - c) * (NumericCast<f32>(x) - c) +
		                                  (NumericCast<f32>(y) - c) * (NumericCast<f32>(y) - c));

	const f32 sAlpha = 1.0f - dist / c;
	const f32 xAlpha = NumericCast<f32>(x) == c ? c : k / TRAP::Math::Abs(NumericCast<f32>(x) - c);
	const f32 yAlpha = NumericCast<f32>(y) == c ? c : k / TRAP::Math::Abs(NumericCast<f32>(y) - c);

	return TRAP::Math::Max(0.0f, TRAP::Math::Min(1.0f, i * sAlpha * 0.2f + sAlpha * xAlpha * yAlpha));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> CursorTests::CreateCursorFrame(const f32 t)
{
	u32 i = 0;
	std::vector<u8> buffer(64ull * 64ull * 4ull, 0);

	for(u32 y = 0; y < 64; y++)
	{
		for(u32 x = 0; x < 64; x++)
		{
			buffer[i++] = 255;
			buffer[i++] = 255;
			buffer[i++] = 255;
			buffer[i++] = NumericCast<u8>(255 * Star(x, y, t));
		}
	}

	return TRAP::Image::LoadFromMemory(64, 64, TRAP::Image::ColorFormat::RGBA, buffer);
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
	for(u32 i = 0; i < 60; i++)
		m_starCursors.push_back(CreateCursorFrame(NumericCast<f32>(i) / 60.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

void CursorTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	if (m_animateCursor)
	{
		const u32 i = NumericCast<u32>(TRAP::Application::GetTime() * 30.0f) % 60;
		if (m_currentFrame != m_starCursors[i].get())
		{
			TRAP::Application::GetWindow()->SetCursorIcon(m_starCursors[i].get(), 32u, 32u);
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
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(this, &CursorTests::OnKeyPress);
	dispatcher.Dispatch<TRAP::Events::MouseMoveEvent>(this, &CursorTests::OnMouseMove);
}

//-------------------------------------------------------------------------------------------------------------------//

bool CursorTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
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
		f32 x = TRAP::Input::GetMouseX();
		f32 y = TRAP::Input::GetMouseY();

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
		const u32 width = TRAP::Application::GetWindow()->GetWidth();
		const u32 height = TRAP::Application::GetWindow()->GetHeight();
		TRAP::Input::SetMousePosition(NumericCast<f32>(width) - 1.0f, NumericCast<f32>(height) - 1.0f);
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
		i32 index = std::to_underlying(event.GetKey()) - std::to_underlying(TRAP::Input::Key::One);
		if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Shift) ||
			TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Shift))
			index += 9;

		if (index <= NumericCast<i32>(std::to_underlying(TRAP::Window::CursorType::NotAllowed)))
			TRAP::Application::GetWindow()->SetCursorType(static_cast<TRAP::Window::CursorType>(index));

		break;
	}

	default:
		break;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool CursorTests::OnMouseMove(const TRAP::Events::MouseMoveEvent& event)
{
	TP_INFO("[Cursor] Position: ", event.GetX(), " ", event.GetY(), " (", event.GetX() - m_cursorX, " ",
	        event.GetY() - m_cursorY, ")");

	m_cursorX = event.GetX();
	m_cursorY = event.GetY();

	return true;
}