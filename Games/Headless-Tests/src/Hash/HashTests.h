#ifndef GAMESTRAP_HASHTESTS_H
#define GAMESTRAP_HASHTESTS_H

#include <TRAP.h>

class HashTests final : public TRAP::Layer
{
public:
	constexpr HashTests();

	void OnAttach() override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr HashTests::HashTests()
	: Layer("Hash")
{
}

#endif /*GAMESTRAP_HASHTESTS_H*/
