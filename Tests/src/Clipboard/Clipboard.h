#ifndef _GAMESTRAP_CLIPBOARD_H_
#define _GAMESTRAP_CLIPBOARD_H_

#include <TRAP.h>

class Clipboard final : public TRAP::Layer
{
public:
	Clipboard();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Event& event) override;

	bool OnKeyPress(TRAP::KeyPressEvent& event);
	
private:
	std::string m_clipboard;
};

#endif /*_GAMESTRAP_CLIPBOARD_H_*/