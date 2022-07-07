#ifndef GAMESTRAP_ANTIALIASINGTESTS_H
#define GAMESTRAP_ANTIALIASINGTESTS_H

#include <TRAP.h>

class AntiAliasingTests final : public TRAP::Layer
{
public:
	AntiAliasingTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;

private:
	TRAP::Graphics::OrthographicCamera m_camera;
};

#endif /*GAMESTRAP_ANTIALIASINGTESTS_H*/