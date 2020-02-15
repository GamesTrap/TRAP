#ifndef _GAMESTRAP_CURSOR_H_
#define _GAMESTRAP_CURSOR_H_

#include <TRAP.h>

class Cursor final : public TRAP::Layer
{
public:
	Cursor();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Event& event) override;

	bool OnKeyPress(TRAP::KeyPressEvent& event);
	bool OnMouseMove(TRAP::MouseMoveEvent& event);
	
private:
	float Star(int32_t x, int32_t y, float t) const;
	TRAP::Scope<TRAP::Image> CreateCursorFrame(float t) const;
	
	float m_cursorX;
	float m_cursorY;
	bool m_animateCursor;
	std::vector<TRAP::Scope<TRAP::Image>> m_starCursors;
	TRAP::Image* m_currentFrame;
};

#endif /*_GAMESTRAP_CURSOR_H_*/