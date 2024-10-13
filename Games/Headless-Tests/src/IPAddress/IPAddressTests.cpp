#include "IPAddressTests.h"

#include <sstream>
#include <array>

namespace
{
    void TestIPv4Construction()
    {
        const auto ipAddress = TRAP::Network::IPv4Address("192.168.0.1");
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::None);
        TRAP_ASSERT(ipAddress.ToString() == "192.168.0.1");
        TRAP_ASSERT(ipAddress.ToInteger() == 0xC0A80001u);
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::Any);
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::Broadcast);
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::LocalHost);

        const auto broadcast = TRAP::Network::IPv4Address("255.255.255.255");
        TRAP_ASSERT(broadcast != TRAP::Network::IPv4Address::None);
        TRAP_ASSERT(broadcast.ToString() == "255.255.255.255");
        TRAP_ASSERT(broadcast.ToInteger() == 0xFFFFFFFFu);
        TRAP_ASSERT(broadcast == TRAP::Network::IPv4Address::Broadcast);

        const auto any = TRAP::Network::IPv4Address("0.0.0.0");
        TRAP_ASSERT(any.ToString() == "0.0.0.0");
        TRAP_ASSERT(any.ToInteger() == 0x00000000u);
        TRAP_ASSERT(any == TRAP::Network::IPv4Address::Any);

        const auto localhost = TRAP::Network::IPv4Address("localhost");
        TRAP_ASSERT(localhost != TRAP::Network::IPv4Address::None);
        TRAP_ASSERT(localhost.ToString() == "127.0.0.1");
        TRAP_ASSERT(localhost.ToInteger() == 0x7F000001u);
        TRAP_ASSERT(localhost == TRAP::Network::IPv4Address::LocalHost);

        TRAP_ASSERT(TRAP::Network::IPv4Address("255.255.255.256") == TRAP::Network::IPv4Address::None);
        TRAP_ASSERT(TRAP::Network::IPv4Address("") == TRAP::Network::IPv4Address::None);

        static constexpr auto ipAddress2 = TRAP::Network::IPv4Address(142u, 250u, 69u, 238u);
        TRAP_ASSERT(ipAddress2.ToString() == "142.250.69.238");
        static_assert(ipAddress2.ToInteger() == 0x8EFA45EEu);

        static constexpr auto ipAddress3 = TRAP::Network::IPv4Address(0xDEADBEEFu);
        TRAP_ASSERT(ipAddress3.ToString() == "222.173.190.239");
        static_assert(ipAddress3.ToInteger() == 0xDEADBEEFu);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void TestIPv6Construction()
    {
        const auto ipAddress = TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::None);
        TRAP_ASSERT(ipAddress.ToString() == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
        static constexpr std::array<u8, 16u> arr{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC0u};
        TRAP_ASSERT(ipAddress.ToArray() == arr);
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::Any);
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::LocalHost);

        const auto any = TRAP::Network::IPv6Address("::");
        TRAP_ASSERT(any.ToString() == "::");
        static constexpr std::array<u8, 16u> arr2{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
        TRAP_ASSERT(any.ToArray() == arr2);
        TRAP_ASSERT(any == TRAP::Network::IPv6Address::Any);

        const auto localhost = TRAP::Network::IPv6Address("::1");
        TRAP_ASSERT(localhost != TRAP::Network::IPv6Address::None);
        TRAP_ASSERT(localhost.ToString() == "::1");
        static constexpr std::array<u8, 16u> arr3{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u};
        TRAP_ASSERT(localhost.ToArray() == arr3);
        TRAP_ASSERT(localhost == TRAP::Network::IPv6Address::LocalHost);

        TRAP_ASSERT(TRAP::Network::IPv6Address("::g") == TRAP::Network::IPv6Address::None);
        TRAP_ASSERT(TRAP::Network::IPv6Address("") == TRAP::Network::IPv6Address::None);

        static constexpr std::array<u8, 16u> arr4{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC0u};
        static constexpr auto ipAddress2 = TRAP::Network::IPv6Address(arr4);
        TRAP_ASSERT(ipAddress2.ToString() == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
        static_assert(ipAddress2.ToArray() == arr4);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void TestIPv4StaticFunctions()
    {
        const auto ipAddress = TRAP::Network::IPv4Address::GetLocalAddress();
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::None);
        TRAP_ASSERT(ipAddress.ToString() != "0.0.0.0");
        TRAP_ASSERT(ipAddress.ToInteger() != 0x00000000u);

        const auto ipAddress2 = TRAP::Network::IPv4Address::GetPublicAddress();
        TRAP_ASSERT(ipAddress2 != TRAP::Network::IPv4Address::None);
        TRAP_ASSERT(ipAddress2.ToString() != "0.0.0.0");
        TRAP_ASSERT(ipAddress2.ToInteger() != 0x00000000u);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void TestIPv6StaticFunctions()
    {
        const auto ipAddress = TRAP::Network::IPv6Address::GetLocalAddress();
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::None);
        TRAP_ASSERT(ipAddress.ToString() != "::");
        static constexpr std::array<u8, 16u> arr{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
        TRAP_ASSERT(ipAddress.ToArray() != arr);

        const auto ipAddress2 = TRAP::Network::IPv6Address::GetPublicAddress();
        TRAP_ASSERT(ipAddress2 != TRAP::Network::IPv6Address::None);
        TRAP_ASSERT(ipAddress2.ToString() != "::");
        TRAP_ASSERT(ipAddress2.ToArray() != arr);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void TestIPv4StaticConstants()
    {
        TRAP_ASSERT(TRAP::Network::IPv4Address::Any.ToString() == "0.0.0.0");
        TRAP_ASSERT(TRAP::Network::IPv4Address::Any.ToInteger() == 0x00000000u);

        TRAP_ASSERT(TRAP::Network::IPv4Address::LocalHost.ToString() == "127.0.0.1");
        TRAP_ASSERT(TRAP::Network::IPv4Address::LocalHost.ToInteger() == 0x7F000001u);

        TRAP_ASSERT(TRAP::Network::IPv4Address::Broadcast.ToString() == "255.255.255.255");
        TRAP_ASSERT(TRAP::Network::IPv4Address::Broadcast.ToInteger() == 0xFFFFFFFFu);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void TestIPv6StaticConstants()
    {
        TRAP_ASSERT(TRAP::Network::IPv6Address::Any.ToString() == "::");
        static constexpr std::array<u8, 16u> arr{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
        TRAP_ASSERT(TRAP::Network::IPv6Address::Any.ToArray() == arr);

        TRAP_ASSERT(TRAP::Network::IPv6Address::LocalHost.ToString() == "::1");
        static constexpr std::array<u8, 16u> arr2{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u};
        TRAP_ASSERT(TRAP::Network::IPv6Address::LocalHost.ToArray() == arr2);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void TestIPv4Operators()
    {
        static_assert(TRAP::Network::IPv4Address(0x42u, 0x69u, 0x96u, 0x24u) == TRAP::Network::IPv4Address(0x42699624u));
        static_assert(TRAP::Network::IPv4Address(0xABCDEF01u) == TRAP::Network::IPv4Address(171u, 205u, 239u, 1u));

        static_assert(TRAP::Network::IPv4Address(0x12344321u) != TRAP::Network::IPv4Address(1234u));
        static_assert(TRAP::Network::IPv4Address(192u, 168u, 1u, 10u) != TRAP::Network::IPv4Address(192u, 168u, 1u, 11u));

        static_assert(TRAP::Network::IPv4Address(1u) < TRAP::Network::IPv4Address(2u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 0u, 0u) < TRAP::Network::IPv4Address(1u, 0u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(1u, 0u, 0u, 0u) < TRAP::Network::IPv4Address(0u, 1u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 1u, 0u, 0u) < TRAP::Network::IPv4Address(0u, 0u, 1u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 1u, 0u) < TRAP::Network::IPv4Address(0u, 0u, 0u, 1u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 0u, 1u) < TRAP::Network::IPv4Address(1u, 0u, 0u, 1u));

        static_assert(TRAP::Network::IPv4Address(2u) > TRAP::Network::IPv4Address(1u));
        static_assert(TRAP::Network::IPv4Address(1u, 0u, 0u, 0u) > TRAP::Network::IPv4Address(0u, 0u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 1u, 0u, 0u) > TRAP::Network::IPv4Address(1u, 0u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 1u, 0u) > TRAP::Network::IPv4Address(0u, 1u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 0u, 1u) > TRAP::Network::IPv4Address(0u, 0u, 1u, 0u));
        static_assert(TRAP::Network::IPv4Address(1u, 0u, 0u, 1u) > TRAP::Network::IPv4Address(0u, 0u, 0u, 1u));

        static_assert(TRAP::Network::IPv4Address(1u) <= TRAP::Network::IPv4Address(2u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 0u, 0u) <= TRAP::Network::IPv4Address(1u, 0u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(1u, 0u, 0u, 0u) <= TRAP::Network::IPv4Address(0u, 1u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 1u, 0u, 0u) <= TRAP::Network::IPv4Address(0u, 0u, 1u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 1u, 0u) <= TRAP::Network::IPv4Address(0u, 0u, 0u, 1u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 0u, 1u) <= TRAP::Network::IPv4Address(1u, 0u, 0u, 1u));
        static_assert(TRAP::Network::IPv4Address(0x42u, 0x69u, 0x96u, 0x24u) <= TRAP::Network::IPv4Address(0x42699624u));
        static_assert(TRAP::Network::IPv4Address(0xABCDEF01u) <= TRAP::Network::IPv4Address(171u, 205u, 239u, 1u));

        static_assert(TRAP::Network::IPv4Address(2u) >= TRAP::Network::IPv4Address(1u));
        static_assert(TRAP::Network::IPv4Address(1u, 0u, 0u, 0u) >= TRAP::Network::IPv4Address(0u, 0u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 1u, 0u, 0u) >= TRAP::Network::IPv4Address(1u, 0u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 1u, 0u) >= TRAP::Network::IPv4Address(0u, 1u, 0u, 0u));
        static_assert(TRAP::Network::IPv4Address(0u, 0u, 0u, 1u) >= TRAP::Network::IPv4Address(0u, 0u, 1u, 0u));
        static_assert(TRAP::Network::IPv4Address(1u, 0u, 0u, 1u) >= TRAP::Network::IPv4Address(0u, 0u, 0u, 1u));
        static_assert(TRAP::Network::IPv4Address(0x42u, 0x69u, 0x96u, 0x24u) >= TRAP::Network::IPv4Address(0x42699624u));
        static_assert(TRAP::Network::IPv4Address(0xABCDEF01u) >= TRAP::Network::IPv4Address(171u, 205u, 239u, 1u));

        TRAP::Network::IPv4Address ipAddress{};
        std::istringstream("4.4.4.4") >> ipAddress;
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv4Address::None);
        TRAP_ASSERT(ipAddress.ToString() == "4.4.4.4");
        TRAP_ASSERT(ipAddress.ToInteger() == 0x04040404u);
        TRAP::Network::IPv4Address ipAddress2{};
        std::istringstream("92.100.0.72") >> ipAddress2;
        TRAP_ASSERT(ipAddress2 != TRAP::Network::IPv4Address::None);
        TRAP_ASSERT(ipAddress2.ToString() == "92.100.0.72");
        TRAP_ASSERT(ipAddress2.ToInteger() == 0x5C640048u);
        TRAP::Network::IPv4Address ipAddress3{};
        std::istringstream("") >> ipAddress3;
        TRAP_ASSERT(ipAddress3 == TRAP::Network::IPv4Address::None);

        TRAP_ASSERT(fmt::format("{}", TRAP::Network::IPv4Address(10u, 9u, 8u, 7u)) == "10.9.8.7");
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void TestIPv6Operators()
    {
        TRAP_ASSERT(TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") == TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0"));
        static constexpr std::array<u8, 16u> arr{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC0u};
        TRAP_ASSERT(TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") == TRAP::Network::IPv6Address(arr));

        TRAP_ASSERT(TRAP::Network::IPv6Address("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") != TRAP::Network::IPv6Address("1234"));
        static constexpr std::array<u8, 16u> arr1{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC1u};
        static_assert(TRAP::Network::IPv6Address(arr) != TRAP::Network::IPv6Address(arr1));

        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}));

        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));

        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}));

        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        static_assert(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));

        TRAP::Network::IPv6Address ipAddress{};
        std::istringstream("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") >> ipAddress;
        TRAP_ASSERT(ipAddress != TRAP::Network::IPv6Address::None);
        TRAP_ASSERT(ipAddress.ToString() == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
        TRAP_ASSERT(ipAddress.ToArray() == arr);
        TRAP::Network::IPv6Address ipAddress2{};
        std::istringstream("::1") >> ipAddress2;
        TRAP_ASSERT(ipAddress2 != TRAP::Network::IPv6Address::None);
        TRAP_ASSERT(ipAddress2.ToString() == "::1");
        static constexpr std::array<u8, 16u> arr2{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u};
        TRAP_ASSERT(ipAddress2.ToArray() == arr2);
        TRAP::Network::IPv6Address ipAddress3{};
        std::istringstream("") >> ipAddress3;
        TRAP_ASSERT(ipAddress3 == TRAP::Network::IPv6Address::None);

        TRAP_ASSERT(fmt::format("{}", TRAP::Network::IPv6Address(arr)) == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
    }
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

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
