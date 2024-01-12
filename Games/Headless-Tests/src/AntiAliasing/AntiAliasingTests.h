#ifndef GAMESTRAP_ANTIALIASINGTESTS_H
#define GAMESTRAP_ANTIALIASINGTESTS_H

#include <TRAP.h>

class AntiAliasingTests final : public TRAP::Layer
{
public:
	constexpr AntiAliasingTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;

private:
	static constexpr TRAP::Graphics::OrthographicCamera s_camera{-(2560.0f / 1440.0f), 2560.0f / 1440.0f, -1.0f, 1.0f, -1.0f, 1.0f};
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr AntiAliasingTests::AntiAliasingTests()
	: Layer("AntiAliasing")
{
}

#endif /*GAMESTRAP_ANTIALIASINGTESTS_H*/