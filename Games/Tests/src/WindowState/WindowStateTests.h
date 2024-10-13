#ifndef GAMESTRAP_MINIMIZETESTS_H
#define GAMESTRAP_MINIMIZETESTS_H

#include <TRAP.h>

class WindowStateTests final : public TRAP::Layer
{
public:
	constexpr WindowStateTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	static constexpr bool OnWindowResize(const TRAP::Events::WindowResizeEvent& event);
	static constexpr bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event);
	static constexpr bool OnWindowFocus(const TRAP::Events::WindowFocusEvent& event);
	static constexpr bool OnWindowLostFocus(const TRAP::Events::WindowLostFocusEvent& event);
	static constexpr bool OnWindowMinimize(const TRAP::Events::WindowMinimizeEvent& event);
	static constexpr bool OnWindowMaximize(const TRAP::Events::WindowMaximizeEvent& event);
	static constexpr bool OnWindowRestore(const TRAP::Events::WindowRestoreEvent& event);
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void WindowStateTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(OnKeyPress);
	dispatcher.Dispatch<TRAP::Events::WindowResizeEvent>(OnWindowResize);
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(OnFrameBufferResize);
	dispatcher.Dispatch<TRAP::Events::WindowFocusEvent>(OnWindowFocus);
	dispatcher.Dispatch<TRAP::Events::WindowLostFocusEvent>(OnWindowLostFocus);
	dispatcher.Dispatch<TRAP::Events::WindowMinimizeEvent>(OnWindowMinimize);
	dispatcher.Dispatch<TRAP::Events::WindowMaximizeEvent>(OnWindowMaximize);
	dispatcher.Dispatch<TRAP::Events::WindowRestoreEvent>(OnWindowRestore);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr WindowStateTests::WindowStateTests()
	: Layer("WindowState")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool WindowStateTests::OnWindowResize(const TRAP::Events::WindowResizeEvent& event)
{
	TP_TRACE(event);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool WindowStateTests::OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event)
{
	TP_TRACE(event);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool WindowStateTests::OnWindowFocus(const TRAP::Events::WindowFocusEvent& event)
{
	TP_TRACE(event);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool WindowStateTests::OnWindowLostFocus(const TRAP::Events::WindowLostFocusEvent& event)
{
	TP_TRACE(event);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool WindowStateTests::OnWindowMinimize(const TRAP::Events::WindowMinimizeEvent& event)
{
	TP_TRACE(event);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool WindowStateTests::OnWindowMaximize(const TRAP::Events::WindowMaximizeEvent& event)
{
	TP_TRACE(event);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool WindowStateTests::OnWindowRestore(const TRAP::Events::WindowRestoreEvent& event)
{
	TP_TRACE(event);

	return true;
}

#endif /*GAMESTRAP_MINIMIZETESTS_H*/
