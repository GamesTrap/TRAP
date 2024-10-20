#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "Network/IP/IPv4Address.h"

TEST_CASE("TRAP::Network::IPv4Address", "[network][ipv4address]")
{
    SECTION("TRAP::Network::IPv4Address()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv4Address{} == TRAP::Network::IPv4Address::None);
    }

    SECTION("TRAP::Network::IPv4Address(std::string)")
    {
        const auto ipAddress = TRAP::Network::IPv4Address("192.168.0.1");
        REQUIRE(ipAddress != TRAP::Network::IPv4Address::None);
        REQUIRE(ipAddress.ToString() == "192.168.0.1");
        REQUIRE(ipAddress.ToInteger() == 0xC0A80001u);
        REQUIRE(ipAddress != TRAP::Network::IPv4Address::Any);
        REQUIRE(ipAddress != TRAP::Network::IPv4Address::Broadcast);
        REQUIRE(ipAddress != TRAP::Network::IPv4Address::LocalHost);

        const auto broadcast = TRAP::Network::IPv4Address("255.255.255.255");
        REQUIRE(broadcast != TRAP::Network::IPv4Address::None);
        REQUIRE(broadcast.ToString() == "255.255.255.255");
        REQUIRE(broadcast.ToInteger() == 0xFFFFFFFFu);
        REQUIRE(broadcast == TRAP::Network::IPv4Address::Broadcast);

        const auto any = TRAP::Network::IPv4Address("0.0.0.0");
        REQUIRE(any.ToString() == "0.0.0.0");
        REQUIRE(any.ToInteger() == 0x00000000u);
        REQUIRE(any == TRAP::Network::IPv4Address::Any);

        const auto localhost = TRAP::Network::IPv4Address("localhost");
        REQUIRE(localhost != TRAP::Network::IPv4Address::None);
        REQUIRE(localhost.ToString() == "127.0.0.1");
        REQUIRE(localhost.ToInteger() == 0x7F000001u);
        REQUIRE(localhost == TRAP::Network::IPv4Address::LocalHost);

        REQUIRE(TRAP::Network::IPv4Address("255.255.255.256") == TRAP::Network::IPv4Address::None);
        REQUIRE(TRAP::Network::IPv4Address("") == TRAP::Network::IPv4Address::None);
    }

    SECTION("TRAP::Network::IPv4Address(u8, u8, u8, u8)")
    {
        static constexpr auto ipAddress2 = TRAP::Network::IPv4Address(142u, 250u, 69u, 238u);
        REQUIRE(ipAddress2.ToString() == "142.250.69.238");
        STATIC_REQUIRE(ipAddress2.ToInteger() == 0x8EFA45EEu);
    }

    SECTION("TRAP::Network::IPv4Address(u32)")
    {
        static constexpr auto ipAddress3 = TRAP::Network::IPv4Address(0xDEADBEEFu);
        REQUIRE(ipAddress3.ToString() == "222.173.190.239");
        STATIC_REQUIRE(ipAddress3.ToInteger() == 0xDEADBEEFu);
    }

    SECTION("TRAP::Network::IPv4Address::GetLocalAddress()")
    {
        const auto ipAddress = TRAP::Network::IPv4Address::GetLocalAddress();
        REQUIRE(ipAddress != TRAP::Network::IPv4Address::None);
        REQUIRE(ipAddress.ToString() != "0.0.0.0");
        REQUIRE(ipAddress.ToInteger() != 0x00000000u);
    }

    SECTION("TRAP::Network::IPv4Address::GetPublicAddress()")
    {
        const auto ipAddress2 = TRAP::Network::IPv4Address::GetPublicAddress(TRAP::Utils::TimeStep(120.0f));
        REQUIRE(ipAddress2 != TRAP::Network::IPv4Address::None);
        REQUIRE(ipAddress2.ToString() != "0.0.0.0");
        REQUIRE(ipAddress2.ToInteger() != 0x00000000u);
    }

    SECTION("Constants")
    {
        REQUIRE(TRAP::Network::IPv4Address::Any.ToString() == "0.0.0.0");
        STATIC_REQUIRE(TRAP::Network::IPv4Address::Any.ToInteger() == 0x00000000u);

        REQUIRE(TRAP::Network::IPv4Address::LocalHost.ToString() == "127.0.0.1");
        STATIC_REQUIRE(TRAP::Network::IPv4Address::LocalHost.ToInteger() == 0x7F000001u);

        REQUIRE(TRAP::Network::IPv4Address::Broadcast.ToString() == "255.255.255.255");
        STATIC_REQUIRE(TRAP::Network::IPv4Address::Broadcast.ToInteger() == 0xFFFFFFFFu);
    }

    SECTION("TRAP::Network::IPv4Address::operator==()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0x42u, 0x69u, 0x96u, 0x24u) == TRAP::Network::IPv4Address(0x42699624u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0xABCDEF01u) == TRAP::Network::IPv4Address(171u, 205u, 239u, 1u));
    }

    SECTION("TRAP::Network::IPv4Address::operator!=()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0x12344321u) != TRAP::Network::IPv4Address(1234u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(192u, 168u, 1u, 10u) != TRAP::Network::IPv4Address(192u, 168u, 1u, 11u));
    }

    SECTION("TRAP::Network::IPv4Address::operator<()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv4Address(1u) < TRAP::Network::IPv4Address(2u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 0u, 0u) < TRAP::Network::IPv4Address(1u, 0u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(1u, 0u, 0u, 0u) < TRAP::Network::IPv4Address(0u, 1u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 1u, 0u, 0u) < TRAP::Network::IPv4Address(0u, 0u, 1u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 1u, 0u) < TRAP::Network::IPv4Address(0u, 0u, 0u, 1u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 0u, 1u) < TRAP::Network::IPv4Address(1u, 0u, 0u, 1u));
    }

    SECTION("TRAP::Network::IPv4Address::operator<=()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv4Address(1u) <= TRAP::Network::IPv4Address(2u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 0u, 0u) <= TRAP::Network::IPv4Address(1u, 0u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(1u, 0u, 0u, 0u) <= TRAP::Network::IPv4Address(0u, 1u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 1u, 0u, 0u) <= TRAP::Network::IPv4Address(0u, 0u, 1u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 1u, 0u) <= TRAP::Network::IPv4Address(0u, 0u, 0u, 1u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 0u, 1u) <= TRAP::Network::IPv4Address(1u, 0u, 0u, 1u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0x42u, 0x69u, 0x96u, 0x24u) <= TRAP::Network::IPv4Address(0x42699624u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0xABCDEF01u) <= TRAP::Network::IPv4Address(171u, 205u, 239u, 1u));
    }

    SECTION("TRAP::Network::IPv4Address::operator>=()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv4Address(2u) >= TRAP::Network::IPv4Address(1u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(1u, 0u, 0u, 0u) >= TRAP::Network::IPv4Address(0u, 0u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 1u, 0u, 0u) >= TRAP::Network::IPv4Address(1u, 0u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 1u, 0u) >= TRAP::Network::IPv4Address(0u, 1u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 0u, 1u) >= TRAP::Network::IPv4Address(0u, 0u, 1u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(1u, 0u, 0u, 1u) >= TRAP::Network::IPv4Address(0u, 0u, 0u, 1u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0x42u, 0x69u, 0x96u, 0x24u) >= TRAP::Network::IPv4Address(0x42699624u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0xABCDEF01u) >= TRAP::Network::IPv4Address(171u, 205u, 239u, 1u));
    }

    SECTION("TRAP::Network::IPv4Address::operator>()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv4Address(2u) > TRAP::Network::IPv4Address(1u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(1u, 0u, 0u, 0u) > TRAP::Network::IPv4Address(0u, 0u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 1u, 0u, 0u) > TRAP::Network::IPv4Address(1u, 0u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 1u, 0u) > TRAP::Network::IPv4Address(0u, 1u, 0u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(0u, 0u, 0u, 1u) > TRAP::Network::IPv4Address(0u, 0u, 1u, 0u));
        STATIC_REQUIRE(TRAP::Network::IPv4Address(1u, 0u, 0u, 1u) > TRAP::Network::IPv4Address(0u, 0u, 0u, 1u));
    }

    SECTION("TRAP::Network::IPv4Address::operator>>()")
    {
        TRAP::Network::IPv4Address ipAddress{};
        std::istringstream("4.4.4.4") >> ipAddress;
        REQUIRE(ipAddress != TRAP::Network::IPv4Address::None);
        REQUIRE(ipAddress.ToString() == "4.4.4.4");
        REQUIRE(ipAddress.ToInteger() == 0x04040404u);

        TRAP::Network::IPv4Address ipAddress2{};
        std::istringstream("92.100.0.72") >> ipAddress2;
        REQUIRE(ipAddress2 != TRAP::Network::IPv4Address::None);
        REQUIRE(ipAddress2.ToString() == "92.100.0.72");
        REQUIRE(ipAddress2.ToInteger() == 0x5C640048u);

        TRAP::Network::IPv4Address ipAddress3{};
        std::istringstream("") >> ipAddress3;
        REQUIRE(ipAddress3 == TRAP::Network::IPv4Address::None);
    }
}
