#ifndef GAMESTRAP_CONTROLLERTESTS_H
#define GAMESTRAP_CONTROLLERTESTS_H

#include <TRAP.h>

class ControllerTests final : public TRAP::Layer
{
public:
	constexpr ControllerTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr ControllerTests::ControllerTests()
	: Layer("Controllers")
{
}

#endif /*GAMESTRAP_CONTROLLERTESTS_H*/
