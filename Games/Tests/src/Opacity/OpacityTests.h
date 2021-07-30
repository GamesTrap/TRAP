#ifndef _GAMESTRAP_OPACITYTESTS_H_
#define _GAMESTRAP_OPACITYTESTS_H_

#include <TRAP.h>

class OpacityTests final : public TRAP::Layer
{
public:
	OpacityTests();

	void OnImGuiRender() override;
	void OnAttach() override;void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);

private:
	float m_opacity = 1.0f;
};

#endif /*_GAMESTRAP_OPACITYTESTS_H_*/