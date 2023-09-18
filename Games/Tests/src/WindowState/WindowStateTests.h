#ifndef GAMESTRAP_MINIMIZETESTS_H
#define GAMESTRAP_MINIMIZETESTS_H

#include <TRAP.h>

class WindowStateTests final : public TRAP::Layer
{
public:
	constexpr WindowStateTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	static bool OnWindowResize(const TRAP::Events::WindowResizeEvent& event);
	static bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event);
	static bool OnWindowFocus(const TRAP::Events::WindowFocusEvent& event);
	static bool OnWindowLostFocus(const TRAP::Events::WindowLostFocusEvent& event);
	static bool OnWindowMinimize(const TRAP::Events::WindowMinimizeEvent& event);
	static bool OnWindowMaximize(const TRAP::Events::WindowMaximizeEvent& event);
	static bool OnWindowRestore(const TRAP::Events::WindowRestoreEvent& event);
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr WindowStateTests::WindowStateTests()
	: Layer("WindowState")
{
}

#endif /*GAMESTRAP_MINIMIZETESTS_H*/