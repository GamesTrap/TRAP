#ifndef GAMESTRAP_TITLETESTS_H
#define GAMESTRAP_TITLETESTS_H

#include <TRAP.h>

class TitleTests final : public TRAP::Layer
{
public:
	constexpr TitleTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr TitleTests::TitleTests()
	: Layer("Title")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TitleTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(OnKeyPress);
}

#endif /*GAMESTRAP_TITLETESTS_H*/
