#ifndef GAMESTRAP_ICONTESTS_H
#define GAMESTRAP_ICONTESTS_H

#include <TRAP.h>

class IconTests final : public TRAP::Layer
{
public:
	constexpr IconTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr IconTests::IconTests()
	: Layer("Icon")
{
}

#endif /*GAMESTRAP_ICONTESTS_H*/
