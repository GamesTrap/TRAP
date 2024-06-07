#ifndef GAMESTRAP_ANTIALIASINGTESTS_H
#define GAMESTRAP_ANTIALIASINGTESTS_H

#include <TRAP.h>

class AntiAliasingTests final : public TRAP::Layer
{
public:
	constexpr AntiAliasingTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr AntiAliasingTests::AntiAliasingTests()
	: Layer("AntiAliasing")
{
}

#endif /*GAMESTRAP_ANTIALIASINGTESTS_H*/
