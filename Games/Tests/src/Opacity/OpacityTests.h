#ifndef GAMESTRAP_OPACITYTESTS_H
#define GAMESTRAP_OPACITYTESTS_H

#include <TRAP.h>

class OpacityTests final : public TRAP::Layer
{
public:
	constexpr OpacityTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	f32 m_opacity = 1.0f;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr OpacityTests::OpacityTests()
	: Layer("Opacity")
{
}

#endif /*GAMESTRAP_OPACITYTESTS_H*/
