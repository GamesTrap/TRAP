#ifndef GAMESTRAP_ICONTESTS_H
#define GAMESTRAP_ICONTESTS_H

#include <TRAP.h>

class IconTests final : public TRAP::Layer
{
public:
	constexpr IconTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr IconTests::IconTests()
	: Layer("Icon")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void IconTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(OnKeyPress);
}

#endif /*GAMESTRAP_ICONTESTS_H*/
