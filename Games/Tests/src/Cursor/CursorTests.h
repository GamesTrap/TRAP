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
	void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	bool OnMouseMove(const TRAP::Events::MouseMoveEvent& event);

private:
	static f32 Star(u32 x, u32 y, f32 t);
	static TRAP::Scope<TRAP::Image> CreateCursorFrame(f32 t);

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

#endif /*GAMESTRAP_CURSORTESTS_H*/