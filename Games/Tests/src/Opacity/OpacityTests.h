#ifndef GAMESTRAP_OPACITYTESTS_H
#define GAMESTRAP_OPACITYTESTS_H

#include <TRAP.h>

class OpacityTests final : public TRAP::Layer
{
public:
	OpacityTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

	static bool OnKeyPress(TRAP::Events::KeyPressEvent& event);

private:
	float m_opacity = 1.0f;
};

#endif /*GAMESTRAP_OPACITYTESTS_H*/