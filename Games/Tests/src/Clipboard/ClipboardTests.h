#ifndef GAMESTRAP_CLIPBOARDTESTS_H
#define GAMESTRAP_CLIPBOARDTESTS_H

#include <TRAP.h>

class ClipboardTests final : public TRAP::Layer
{
public:
	constexpr ClipboardTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

private:
	std::string m_clipboard;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr ClipboardTests::ClipboardTests()
	: Layer("Clipboard")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void ClipboardTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&ClipboardTests::OnKeyPress, this));
}

#endif /*GAMESTRAP_CLIPBOARDTESTS_H*/
