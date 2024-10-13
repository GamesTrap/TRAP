#ifndef GAMESTRAP_IPADDRESSTESTS_H
#define GAMESTRAP_IPADDRESSTESTS_H

#include <TRAP.h>

class IPAddressTests final : public TRAP::Layer
{
public:
	constexpr IPAddressTests();

	void OnAttach() override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr IPAddressTests::IPAddressTests()
	: Layer("IPAddress")
{
}

#endif /*GAMESTRAP_IPADDRESSTESTS_H*/
