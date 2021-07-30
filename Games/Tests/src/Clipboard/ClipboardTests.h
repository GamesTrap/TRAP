#ifndef _GAMESTRAP_CLIPBOARDTESTS_H_
#define _GAMESTRAP_CLIPBOARDTESTS_H_

#include <TRAP.h>

class ClipboardTests final : public TRAP::Layer
{
public:
	ClipboardTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);

private:
	std::string m_clipboard;
};

#endif /*_GAMESTRAP_CLIPBOARDTESTS_H_*/