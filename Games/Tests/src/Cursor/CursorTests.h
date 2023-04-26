#ifndef GAMESTRAP_CURSORTESTS_H
#define GAMESTRAP_CURSORTESTS_H

#include <TRAP.h>

class CursorTests final : public TRAP::Layer
{
public:
	CursorTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
	bool OnMouseMove(TRAP::Events::MouseMoveEvent& event);

private:
	static float Star(uint32_t x, uint32_t y, float t);
	static TRAP::Scope<TRAP::Image> CreateCursorFrame(float t);

	float m_cursorX;
	float m_cursorY;
	bool m_animateCursor;
	std::vector<TRAP::Scope<TRAP::Image>> m_starCursors;
	TRAP::Image* m_currentFrame;
};

#endif /*GAMESTRAP_CURSORTESTS_H*/