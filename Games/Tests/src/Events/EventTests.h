#ifndef GAMESTRAP_EVENTTESTS_H
#define GAMESTRAP_EVENTTESTS_H

#include <TRAP.h>

class EventTests final : public TRAP::Layer
{
public:
	constexpr EventTests();

	void OnAttach() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr EventTests::EventTests()
	: Layer("Events")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void EventTests::OnEvent(TRAP::Events::Event& event)
{
	TP_TRACE(event);
}

#endif /*GAMESTRAP_EVENTTESTS_H*/
