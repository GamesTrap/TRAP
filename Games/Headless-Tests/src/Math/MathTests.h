#ifndef GAMESTRAP_MATHTESTS_H
#define GAMESTRAP_MATHTESTS_H

#include <TRAP.h>

class MathTests final : public TRAP::Layer
{
public:
	MathTests();

	void OnAttach() override;
};

#endif /*GAMESTRAP_MATHTESTS_H*/