#ifndef GAMESTRAP_IPADDRESSTESTS_H
#define GAMESTRAP_IPADDRESSTESTS_H

#include <TRAP.h>

class IPAddressTests final : public TRAP::Layer
{
public:
	constexpr IPAddressTests();

	void OnAttach() override;

private:
    void TestIPv4Construction();
    void TestIPv6Construction();

    void TestIPv4StaticFunctions();
    void TestIPv6StaticFunctions();

    void TestIPv4StaticConstants();
    void TestIPv6StaticConstants();

    void TestIPv4Operators();
    void TestIPv6Operators();
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr IPAddressTests::IPAddressTests()
	: Layer("IPAddress")
{
}

#endif /*GAMESTRAP_IPADDRESSTESTS_H*/