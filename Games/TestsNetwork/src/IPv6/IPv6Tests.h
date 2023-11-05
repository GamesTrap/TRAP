#ifndef GAMESTRAP_IPV6TESTS_H
#define GAMESTRAP_IPV6TESTS_H

#include <TRAP.h>

class IPv6Tests final : public TRAP::Layer
{
public:
	constexpr IPv6Tests();

	void OnAttach() override;

private:
	static void IPv6();
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr IPv6Tests::IPv6Tests()
	: Layer("IPv6")
{
}

#endif /*GAMESTRAP_IPV6TESTS_H*/