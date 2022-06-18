#ifndef _GAMESTRAP_TITLETESTS_H_
#define _GAMESTRAP_TITLETESTS_H_

#include <TRAP.h>

class TitleTests final : public TRAP::Layer
{
public:
	TitleTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
};

#endif /*_GAMESTRAP_TITLETESTS_H_*/