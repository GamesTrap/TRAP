#include "IPAddressTests.h"

#include <sstream>
#include <array>

void IPAddressTests::OnAttach()
{
    TestIPv4Construction();
    TestIPv6Construction();

    TestIPv4StaticFunctions();
    TestIPv6StaticFunctions();

    TestIPv4StaticConstants();
    TestIPv6StaticConstants();

    TestIPv4Operators();
    TestIPv6Operators();

	TRAP::Application::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void IPAddressTests::TestIPv4Construction()
{
    const auto ipAddress = TRAP::Network::IPv4Address("192.168.0.1");
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::None);
    TRAP_ASSERT(ipAddress.ToString() == "192.168.0.1");
    TRAP_ASSERT(ipAddress.ToInteger() == 0xC0A80001);
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::Any);
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::Broadcast);
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::LocalHost);

    const auto broadcast = TRAP::Network::IPv4Address("255.255.255.255");
    TRAP_ASSERT(broadcast != TRAP::Network::IPv4Address::None);
    TRAP_ASSERT(broadcast.ToString() == "255.255.255.255");
    TRAP_ASSERT(broadcast.ToInteger() == 0xFFFFFFFF);
    TRAP_ASSERT(broadcast == TRAP::Network::IPv4Address::Broadcast);

    const auto any = TRAP::Network::IPv4Address("0.0.0.0");
    TRAP_ASSERT(any.ToString() == "0.0.0.0");
    TRAP_ASSERT(any.ToInteger() == 0x00000000);
    TRAP_ASSERT(any == TRAP::Network::IPv4Address::Any);

    const auto localhost = TRAP::Network::IPv4Address("localhost");
    TRAP_ASSERT(localhost != TRAP::Network::IPv4Address::None);
    TRAP_ASSERT(localhost.ToString() == "127.0.0.1");
    TRAP_ASSERT(localhost.ToInteger() == 0x7F000001);
    TRAP_ASSERT(localhost == TRAP::Network::IPv4Address::LocalHost);

    TRAP_ASSERT(TRAP::Network::IPv4Address("255.255.255.256") == TRAP::Network::IPv4Address::None);
    TRAP_ASSERT(TRAP::Network::IPv4Address("") == TRAP::Network::IPv4Address::None);

    const auto ipAddress2 = TRAP::Network::IPv4Address(142, 250, 69, 238);
    TRAP_ASSERT(ipAddress2.ToString() == "142.250.69.238");
    TRAP_ASSERT(ipAddress2.ToInteger() == 0x8EFA45EE);

    const auto ipAddress3 = TRAP::Network::IPv4Address(0xDEADBEEF);
    TRAP_ASSERT(ipAddress3.ToString() == "222.173.190.239");
    TRAP_ASSERT(ipAddress3.ToInteger() == 0xDEADBEEF);
}

//-------------------------------------------------------------------------------------------------------------------//

void IPAddressTests::TestIPv6Construction()
{
    const auto ipAddress = TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::None);
    TRAP_ASSERT(ipAddress.ToString() == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
    static constexpr std::array<u8, 16> arr{0x2A, 0x02, 0x09, 0x08, 0x22, 0x26, 0x12, 0x20, 0xA2, 0xE6, 0x22, 0xC4, 0x86, 0xBD, 0x20, 0xC0};
    TRAP_ASSERT(ipAddress.ToArray() == arr);
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::Any);
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::LocalHost);

    const auto any = TRAP::Network::IPv6Address("::");
    TRAP_ASSERT(any.ToString() == "::");
    static constexpr std::array<u8, 16> arr2{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    TRAP_ASSERT(any.ToArray() == arr2);
    TRAP_ASSERT(any == TRAP::Network::IPv6Address::Any);

    const auto localhost = TRAP::Network::IPv6Address("::1");
    TRAP_ASSERT(localhost != TRAP::Network::IPv6Address::None);
    TRAP_ASSERT(localhost.ToString() == "::1");
    static constexpr std::array<u8, 16> arr3{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    TRAP_ASSERT(localhost.ToArray() == arr3);
    TRAP_ASSERT(localhost == TRAP::Network::IPv6Address::LocalHost);

    TRAP_ASSERT(TRAP::Network::IPv6Address("::g") == TRAP::Network::IPv6Address::None);
    TRAP_ASSERT(TRAP::Network::IPv6Address("") == TRAP::Network::IPv6Address::None);

    static constexpr std::array<u8, 16> arr4{0x2A, 0x02, 0x09, 0x08, 0x22, 0x26, 0x12, 0x20, 0xA2, 0xE6, 0x22, 0xC4, 0x86, 0xBD, 0x20, 0xC0};
    const auto ipAddress2 = TRAP::Network::IPv6Address(arr4);
    TRAP_ASSERT(ipAddress2.ToString() == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
    TRAP_ASSERT(ipAddress2.ToArray() == arr4);
}

//-------------------------------------------------------------------------------------------------------------------//

void IPAddressTests::TestIPv4StaticFunctions()
{
    const auto ipAddress = TRAP::Network::IPv4Address::GetLocalAddress();
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::None);
    TRAP_ASSERT(ipAddress.ToString() != "0.0.0.0");
    TRAP_ASSERT(ipAddress.ToInteger() != 0x00000000);

    const auto ipAddress2 = TRAP::Network::IPv4Address::GetPublicAddress();
    TRAP_ASSERT(ipAddress2 != TRAP::Network::IPv4Address::None);
    TRAP_ASSERT(ipAddress2.ToString() != "0.0.0.0");
    TRAP_ASSERT(ipAddress2.ToInteger() != 0x00000000);
}

//-------------------------------------------------------------------------------------------------------------------//

void IPAddressTests::TestIPv6StaticFunctions()
{
    const auto ipAddress = TRAP::Network::IPv6Address::GetLocalAddress();
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::None);
    TRAP_ASSERT(ipAddress.ToString() != "::");
    static constexpr std::array<u8, 16> arr{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    TRAP_ASSERT(ipAddress.ToArray() != arr);

    const auto ipAddress2 = TRAP::Network::IPv6Address::GetPublicAddress();
    TRAP_ASSERT(ipAddress2 != TRAP::Network::IPv6Address::None);
    TRAP_ASSERT(ipAddress2.ToString() != "::");
    TRAP_ASSERT(ipAddress2.ToArray() != arr);
}

//-------------------------------------------------------------------------------------------------------------------//

void IPAddressTests::TestIPv4StaticConstants()
{
    TRAP_ASSERT(TRAP::Network::IPv4Address::Any.ToString() == "0.0.0.0");
    TRAP_ASSERT(TRAP::Network::IPv4Address::Any.ToInteger() == 0x00000000);

    TRAP_ASSERT(TRAP::Network::IPv4Address::LocalHost.ToString() == "127.0.0.1");
    TRAP_ASSERT(TRAP::Network::IPv4Address::LocalHost.ToInteger() == 0x7F000001);

    TRAP_ASSERT(TRAP::Network::IPv4Address::Broadcast.ToString() == "255.255.255.255");
    TRAP_ASSERT(TRAP::Network::IPv4Address::Broadcast.ToInteger() == 0xFFFFFFFF);
}

//-------------------------------------------------------------------------------------------------------------------//

void IPAddressTests::TestIPv6StaticConstants()
{
    TRAP_ASSERT(TRAP::Network::IPv6Address::Any.ToString() == "::");
    static constexpr std::array<u8, 16> arr{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    TRAP_ASSERT(TRAP::Network::IPv6Address::Any.ToArray() == arr);

    TRAP_ASSERT(TRAP::Network::IPv6Address::LocalHost.ToString() == "::1");
    static constexpr std::array<u8, 16> arr2{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    TRAP_ASSERT(TRAP::Network::IPv6Address::LocalHost.ToArray() == arr2);
}

//-------------------------------------------------------------------------------------------------------------------//

void IPAddressTests::TestIPv4Operators()
{
    TRAP_ASSERT(TRAP::Network::IPv4Address(0x42, 0x69, 0x96, 0x24) == TRAP::Network::IPv4Address(0x42699624));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0xABCDEF01) == TRAP::Network::IPv4Address(171, 205, 239, 1));

    TRAP_ASSERT(TRAP::Network::IPv4Address(0x12344321) != TRAP::Network::IPv4Address(1234));
    TRAP_ASSERT(TRAP::Network::IPv4Address(192, 168, 1, 10) != TRAP::Network::IPv4Address(192, 168, 1, 11));

    TRAP_ASSERT(TRAP::Network::IPv4Address(1) < TRAP::Network::IPv4Address(2));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 0, 0) < TRAP::Network::IPv4Address(1, 0, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(1, 0, 0, 0) < TRAP::Network::IPv4Address(0, 1, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 1, 0, 0) < TRAP::Network::IPv4Address(0, 0, 1, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 1, 0) < TRAP::Network::IPv4Address(0, 0, 0, 1));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 0, 1) < TRAP::Network::IPv4Address(1, 0, 0, 1));

    TRAP_ASSERT(TRAP::Network::IPv4Address(2) > TRAP::Network::IPv4Address(1));
    TRAP_ASSERT(TRAP::Network::IPv4Address(1, 0, 0, 0) > TRAP::Network::IPv4Address(0, 0, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 1, 0, 0) > TRAP::Network::IPv4Address(1, 0, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 1, 0) > TRAP::Network::IPv4Address(0, 1, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 0, 1) > TRAP::Network::IPv4Address(0, 0, 1, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(1, 0, 0, 1) > TRAP::Network::IPv4Address(0, 0, 0, 1));

    TRAP_ASSERT(TRAP::Network::IPv4Address(1) <= TRAP::Network::IPv4Address(2));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 0, 0) <= TRAP::Network::IPv4Address(1, 0, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(1, 0, 0, 0) <= TRAP::Network::IPv4Address(0, 1, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 1, 0, 0) <= TRAP::Network::IPv4Address(0, 0, 1, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 1, 0) <= TRAP::Network::IPv4Address(0, 0, 0, 1));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 0, 1) <= TRAP::Network::IPv4Address(1, 0, 0, 1));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0x42, 0x69, 0x96, 0x24) <= TRAP::Network::IPv4Address(0x42699624));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0xABCDEF01) <= TRAP::Network::IPv4Address(171, 205, 239, 1));

    TRAP_ASSERT(TRAP::Network::IPv4Address(2) >= TRAP::Network::IPv4Address(1));
    TRAP_ASSERT(TRAP::Network::IPv4Address(1, 0, 0, 0) >= TRAP::Network::IPv4Address(0, 0, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 1, 0, 0) >= TRAP::Network::IPv4Address(1, 0, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 1, 0) >= TRAP::Network::IPv4Address(0, 1, 0, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0, 0, 0, 1) >= TRAP::Network::IPv4Address(0, 0, 1, 0));
    TRAP_ASSERT(TRAP::Network::IPv4Address(1, 0, 0, 1) >= TRAP::Network::IPv4Address(0, 0, 0, 1));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0x42, 0x69, 0x96, 0x24) >= TRAP::Network::IPv4Address(0x42699624));
    TRAP_ASSERT(TRAP::Network::IPv4Address(0xABCDEF01) >= TRAP::Network::IPv4Address(171, 205, 239, 1));

    TRAP::Network::IPv4Address ipAddress{};
    std::istringstream("4.4.4.4") >> ipAddress;
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::None);
    TRAP_ASSERT(ipAddress.ToString() == "4.4.4.4");
    TRAP_ASSERT(ipAddress.ToInteger() == 0x04040404);
    TRAP::Network::IPv4Address ipAddress2{};
    std::istringstream("92.100.0.72") >> ipAddress2;
    TRAP_ASSERT(ipAddress2 != TRAP::Network::IPv4Address::None);
    TRAP_ASSERT(ipAddress2.ToString() == "92.100.0.72");
    TRAP_ASSERT(ipAddress2.ToInteger() == 0x5C640048);
    TRAP::Network::IPv4Address ipAddress3{};
    std::istringstream("") >> ipAddress3;
    TRAP_ASSERT(ipAddress3 == TRAP::Network::IPv4Address::None);

    TRAP_ASSERT(fmt::format("{}", TRAP::Network::IPv4Address(10, 9, 8, 7)) == "10.9.8.7");
}

//-------------------------------------------------------------------------------------------------------------------//

void IPAddressTests::TestIPv6Operators()
{
    TRAP_ASSERT(TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") == TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0"));
    static constexpr std::array<u8, 16> arr{0x2A, 0x02, 0x09, 0x08, 0x22, 0x26, 0x12, 0x20, 0xA2, 0xE6, 0x22, 0xC4, 0x86, 0xBD, 0x20, 0xC0};
    TRAP_ASSERT(TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") == TRAP::Network::IPv6Address(arr));

    TRAP_ASSERT(TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") != TRAP::Network::IPv6Address("1234"));
    static constexpr std::array<u8, 16> arr1{0x2A, 0x02, 0x09, 0x08, 0x22, 0x26, 0x12, 0x20, 0xA2, 0xE6, 0x22, 0xC4, 0x86, 0xBD, 0x20, 0xC1};
    TRAP_ASSERT(TRAP::Network::IPv6Address(arr) != TRAP::Network::IPv6Address(arr1));

    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) < TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));

    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) > TRAP::Network::IPv6Address(std::array<u8, 16>{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}) > TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) <= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));

    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    TRAP_ASSERT(TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}) >= TRAP::Network::IPv6Address(std::array<u8, 16>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

    TRAP::Network::IPv6Address ipAddress{};
    std::istringstream("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") >> ipAddress;
    TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::None);
    TRAP_ASSERT(ipAddress.ToString() == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
    TRAP_ASSERT(ipAddress.ToArray() == arr);
    TRAP::Network::IPv6Address ipAddress2{};
    std::istringstream("::1") >> ipAddress2;
    TRAP_ASSERT(ipAddress2 != TRAP::Network::IPv6Address::None);
    TRAP_ASSERT(ipAddress2.ToString() == "::1");
    static constexpr std::array<u8, 16> arr2{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    TRAP_ASSERT(ipAddress2.ToArray() == arr2);
    TRAP::Network::IPv6Address ipAddress3{};
    std::istringstream("") >> ipAddress3;
    TRAP_ASSERT(ipAddress3 == TRAP::Network::IPv6Address::None);

    TRAP_ASSERT(fmt::format("{}", TRAP::Network::IPv6Address(arr)) == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
}