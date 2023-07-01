#ifndef GAMESTRAP_CLIPBOARDTESTS_H
#define GAMESTRAP_CLIPBOARDTESTS_H

#include <TRAP.h>

class ClipboardTests final : public TRAP::Layer
{
public:
	ClipboardTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

private:
	std::string m_clipboard;
};

#endif /*GAMESTRAP_CLIPBOARDTESTS_H*/