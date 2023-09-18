#ifndef GAMESTRAP_EVENTTESTS_H
#define GAMESTRAP_EVENTTESTS_H

#include <TRAP.h>

class EventTests final : public TRAP::Layer
{
public:
	constexpr EventTests();

	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr EventTests::EventTests()
	: Layer("Events")
{
}

#endif /*GAMESTRAP_EVENTTESTS_H*/