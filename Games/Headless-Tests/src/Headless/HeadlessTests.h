#ifndef GAMESTRAP_HEADLESSTESTS_H
#define GAMESTRAP_HEADLESSTESTS_H

#include <TRAP.h>

class HeadlessTests final : public TRAP::Layer
{
public:
	constexpr HeadlessTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr HeadlessTests::HeadlessTests()
	: Layer("Headless")
{
}

#endif /*GAMESTRAP_HEADLESSTESTS_H*/