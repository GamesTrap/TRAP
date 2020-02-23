#include "CursorTests.h"

float CursorTests::Star(const int32_t x, const int32_t y, const float t) const
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

TRAP::Scope<TRAP::Image> CursorTests::CreateCursorFrame(const float t) const
{
	int32_t i = 0;
	std::vector<uint8_t> buffer(64 * 64 * 4, 0);

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

	return TRAP::Image::LoadFromMemory(64, 64, 32, TRAP::ImageFormat::RGBA, buffer);
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

void CursorTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Cursor");

	m_starCursors.reserve(60);
	for(uint32_t i = 0; i < 60; i++)
		m_starCursors.push_back(CreateCursorFrame(i / 60.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

void CursorTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color);

	if (m_animateCursor)
	{
		const int32_t i = static_cast<int32_t>(TRAP::Application::GetTime() * 30.0f) % 60;
		if (m_currentFrame != m_starCursors[i].get())
		{
			TRAP::Application::GetWindow()->SetCursorIcon(m_starCursors[i], 32, 32);
			m_currentFrame = m_starCursors[i].get();
		}
	}
	else
		m_currentFrame = nullptr;

}

//-------------------------------------------------------------------------------------------------------------------//

void CursorTests::OnEvent(TRAP::Event& event)
{
	TRAP::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::KeyPressEvent>([this](TRAP::KeyPressEvent& e) { return OnKeyPress(e); });
	dispatcher.Dispatch<TRAP::MouseMoveEvent>([this](TRAP::MouseMoveEvent& e) { return OnMouseMove(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool CursorTests::OnKeyPress(TRAP::KeyPressEvent& event)
{
	if (event.GetRepeatCount() == 0)
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
			const int32_t width = TRAP::Application::GetWindow()->GetWidth();
			const int32_t height = TRAP::Application::GetWindow()->GetHeight();
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
		case TRAP::Input::Key::Two:
		case TRAP::Input::Key::Three:
		case TRAP::Input::Key::Four:
		case TRAP::Input::Key::Five:
		case TRAP::Input::Key::Six:
		case TRAP::Input::Key::Seven:
		case TRAP::Input::Key::Eight:
		case TRAP::Input::Key::Nine:
		{
			int32_t index = static_cast<int32_t>(event.GetKey()) - static_cast<int32_t>(TRAP::Input::Key::One);
			if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Shift) || TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Shift))
				index += 9;

			if (index <= static_cast<int32_t>(TRAP::Window::CursorType::NotAllowed))
				TRAP::Application::GetWindow()->SetCursorType(static_cast<TRAP::Window::CursorType>(index));

			break;
		}

		default:
			break;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool CursorTests::OnMouseMove(TRAP::MouseMoveEvent& event)
{
	TP_INFO("[Cursor] Position: ", event.GetX(), " ", event.GetY(), " (", event.GetX() - m_cursorX, " ", event.GetY() - m_cursorY, ")");
	
	m_cursorX = event.GetX();
	m_cursorY = event.GetY();

	return true;
}