#ifndef GAMESTRAP_SOCKETTESTS_H
#define GAMESTRAP_SOCKETTESTS_H

#include <TRAP.h>

class SocketTests final : public TRAP::Layer
{
public:
	constexpr SocketTests();

	void OnAttach() override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr SocketTests::SocketTests()
	: Layer("SocketTests")
{
}

#endif /*GAMESTRAP_SOCKETTESTS_H*/
