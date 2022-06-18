#ifndef _GAMESTRAP_EVENTTESTS_H_
#define _GAMESTRAP_EVENTTESTS_H_

#include <TRAP.h>

class EventTests final : public TRAP::Layer
{
public:
	EventTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;
};

#endif /*_GAMESTRAP_EVENTTESTS_H_*/