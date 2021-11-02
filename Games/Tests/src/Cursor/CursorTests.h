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
	float Star(int32_t x, int32_t y, float t) const;
	TRAP::Scope<TRAP::Image> CreateCursorFrame(float t) const;

	float m_cursorX;
	float m_cursorY;
	bool m_animateCursor;
	std::vector<TRAP::Scope<TRAP::Image>> m_starCursors;
	TRAP::Image* m_currentFrame;
};

#endif /*GAMESTRAP_CURSORTESTS_H*/