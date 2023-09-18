#ifndef GAMESTRAP_TITLETESTS_H
#define GAMESTRAP_TITLETESTS_H

#include <TRAP.h>

class TitleTests final : public TRAP::Layer
{
public:
	constexpr TitleTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr TitleTests::TitleTests()
	: Layer("Title")
{
}

#endif /*GAMESTRAP_TITLETESTS_H*/