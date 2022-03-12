#ifndef GAMESTRAP_HEADLESSTESTS_H
#define GAMESTRAP_HEADLESSTESTS_H

#include <TRAP.h>

class HeadlessTests final : public TRAP::Layer
{
public:
	HeadlessTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
};

#endif /*GAMESTRAP_HEADLESSTESTS_H*/