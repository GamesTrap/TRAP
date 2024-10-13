#ifndef GAMESTRAP_OPACITYTESTS_H
#define GAMESTRAP_OPACITYTESTS_H

#include <TRAP.h>

class OpacityTests final : public TRAP::Layer
{
public:
	constexpr OpacityTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

	f32 m_opacity = 1.0f;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr OpacityTests::OpacityTests()
	: Layer("Opacity")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void OpacityTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(OnKeyPress);
}

#endif /*GAMESTRAP_OPACITYTESTS_H*/
