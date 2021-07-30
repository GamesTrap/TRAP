#ifndef _GAMESTRAP_TITLETESTS_H_
#define _GAMESTRAP_TITLETESTS_H_

#include <TRAP.h>

class TitleTests final : public TRAP::Layer
{
public:
	TitleTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
};

#endif /*_GAMESTRAP_TITLETESTS_H_*/