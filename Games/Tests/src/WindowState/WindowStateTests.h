#ifndef GAMESTRAP_MINIMIZETESTS_H
#define GAMESTRAP_MINIMIZETESTS_H

#include <TRAP.h>

class WindowStateTests final : public TRAP::Layer
{
public:
	constexpr WindowStateTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr WindowStateTests::WindowStateTests()
	: Layer("WindowState")
{
}

#endif /*GAMESTRAP_MINIMIZETESTS_H*/
