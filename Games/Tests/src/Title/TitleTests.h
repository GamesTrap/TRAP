#ifndef GAMESTRAP_TITLETESTS_H
#define GAMESTRAP_TITLETESTS_H

#include <TRAP.h>

class TitleTests final : public TRAP::Layer
{
public:
	TitleTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

	static bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
};

#endif /*GAMESTRAP_TITLETESTS_H*/