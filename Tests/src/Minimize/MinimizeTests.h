#ifndef _GAMESTRAP_MINIMIZETESTS_H_
#define _GAMESTRAP_MINIMIZETESTS_H_

#include <TRAP.h>

class MinimizeTests final : public TRAP::Layer
{
public:
	MinimizeTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Event& event) override;

private:
	bool OnKeyPress(const TRAP::KeyPressEvent& event);
	bool OnWindowResize(const TRAP::WindowResizeEvent& event);
	bool OnFrameBufferResize(const TRAP::FrameBufferResizeEvent& event);
	bool OnWindowFocus(const TRAP::WindowFocusEvent& event);
	bool OnWindowLostFocus(const TRAP::WindowLostFocusEvent& event);
};

#endif /*_GAMESTRAP_MINIMIZETESTS_H_*/