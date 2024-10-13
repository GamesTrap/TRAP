#ifndef GAMESTRAP_CURSORTESTS_H
#define GAMESTRAP_CURSORTESTS_H

#include <TRAP.h>

class CursorTests final : public TRAP::Layer
{
public:
	constexpr CursorTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	constexpr bool OnMouseMove(const TRAP::Events::MouseMoveEvent& event);

private:
	f32 m_cursorX = 0.0f;
	f32 m_cursorY = 0.0f;
	bool m_animateCursor = false;
	std::vector<TRAP::Scope<TRAP::Image>> m_starCursors{};
	TRAP::Image* m_currentFrame = nullptr;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr CursorTests::CursorTests()
	: Layer("Cursor")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void CursorTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&CursorTests::OnKeyPress, this));
	dispatcher.Dispatch<TRAP::Events::MouseMoveEvent>(std::bind_front(&CursorTests::OnMouseMove, this));
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool CursorTests::OnMouseMove(const TRAP::Events::MouseMoveEvent& event)
{
	TP_INFO("[Cursor] Position: ", event.GetX(), " ", event.GetY(), " (", event.GetX() - m_cursorX, " ",
	        event.GetY() - m_cursorY, ")");

	m_cursorX = event.GetX();
	m_cursorY = event.GetY();

	return true;
}

#endif /*GAMESTRAP_CURSORTESTS_H*/
