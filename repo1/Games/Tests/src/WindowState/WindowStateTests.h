#ifndef _GAMESTRAP_MINIMIZETESTS_H_
#define _GAMESTRAP_MINIMIZETESTS_H_

#include <TRAP.h>

class WindowStateTests final : public TRAP::Layer
{
public:
	WindowStateTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	bool OnWindowResize(const TRAP::Events::WindowResizeEvent& event);
	bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event);
	bool OnWindowFocus(const TRAP::Events::WindowFocusEvent& event);
	bool OnWindowLostFocus(const TRAP::Events::WindowLostFocusEvent& event);
	bool OnWindowMinimize(const TRAP::Events::WindowMinimizeEvent& event);
	bool OnWindowMaximize(const TRAP::Events::WindowMaximizeEvent& event);
	bool OnWindowRestore(const TRAP::Events::WindowRestoreEvent& event);
};

#endif /*_GAMESTRAP_MINIMIZETESTS_H_*/