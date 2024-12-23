#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "Network/IP/IPv6Address.h"

TEST_CASE("TRAP::Network::IPv6Address", "[network][ipv6address]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE_FALSE(std::is_final_v<TRAP::Network::IPv6Address>);
        STATIC_REQUIRE(std::copyable<TRAP::Network::IPv6Address>);
        STATIC_REQUIRE(std::movable<TRAP::Network::IPv6Address>);
    }

    SECTION("TRAP::Network::IPv6Address(std::string)")
    {
        const auto ipAddress = TRAP::Network::IPv6Address::Resolve("2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
        REQUIRE(ipAddress);
        REQUIRE(ipAddress->ToString() == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
        static constexpr std::array<u8, 16u> arr{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC0u};
        REQUIRE(ipAddress->ToArray() == arr);
        REQUIRE_FALSE(*ipAddress == TRAP::Network::IPv6Address::Any);
        REQUIRE_FALSE(*ipAddress == TRAP::Network::IPv6Address::LocalHost);

        const auto any = TRAP::Network::IPv6Address::Resolve("::");
        REQUIRE(any->ToString() == "::");
        static constexpr std::array<u8, 16u> arr2{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
        REQUIRE(any->ToArray() == arr2);
        REQUIRE(*any == TRAP::Network::IPv6Address::Any);

        const auto localhost = TRAP::Network::IPv6Address::Resolve("::1");
        REQUIRE(localhost);
        REQUIRE(localhost->ToString() == "::1");
        static constexpr std::array<u8, 16u> arr3{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u};
        REQUIRE(localhost->ToArray() == arr3);
        REQUIRE(*localhost == TRAP::Network::IPv6Address::LocalHost);

        REQUIRE_FALSE(TRAP::Network::IPv6Address::Resolve("::g"));
        REQUIRE_FALSE(TRAP::Network::IPv6Address::Resolve(""));
    }

    SECTION("TRAP::Network::IPv6Address(std::array<u8, 16u>)")
    {
        static constexpr std::array<u8, 16u> arr4{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC0u};
        static constexpr auto ipAddress2 = TRAP::Network::IPv6Address(arr4);
        REQUIRE(ipAddress2.ToString() == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
        STATIC_REQUIRE(ipAddress2.ToArray() == arr4);
    }

    SECTION("TRAP::Network::IPv6Address::GetLocalAddress()")
    {
        [[maybe_unused]] const auto ipAddress = TRAP::Network::IPv6Address::GetLocalAddress();
        // CHECK(ipAddress != TRAP::Network::IPv6Address::None);
        // CHECK(ipAddress.ToString() != "::");
        // static constexpr std::array<u8, 16u> arr{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
        // CHECK(ipAddress.ToArray() != arr);
    }

    // SECTION("TRAP::Network::IPv6Address::GetPublicAddress()")
    // {
    //     // [[maybe_unused]] const auto ipAddress2 = TRAP::Network::IPv6Address::GetPublicAddress(TRAP::Utils::TimeStep(120.0f));
    //     // CHECK(ipAddress2 != TRAP::Network::IPv6Address::None);
    //     // CHECK(ipAddress2.ToString() != "::");
    //     // static constexpr std::array<u8, 16u> arr{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
    //     // CHECK(ipAddress2.ToArray() != arr);
    // }

    SECTION("Constants")
    {
        REQUIRE(TRAP::Network::IPv6Address::Any.ToString() == "::");
        static constexpr std::array<u8, 16u> arr{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
        STATIC_REQUIRE(TRAP::Network::IPv6Address::Any.ToArray() == arr);

        REQUIRE(TRAP::Network::IPv6Address::LocalHost.ToString() == "::1");
        static constexpr std::array<u8, 16u> arr2{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u};
        STATIC_REQUIRE(TRAP::Network::IPv6Address::LocalHost.ToArray() == arr2);
    }

    SECTION("TRAP::Network::IPv6Address::operator==()")
    {
        REQUIRE(TRAP::Network::IPv6Address::Resolve("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") == TRAP::Network::IPv6Address::Resolve("2a02:908:2226:1220:a2e6:22c4:86bd:20c0"));
        static constexpr std::array<u8, 16u> arr{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC0u};
        REQUIRE(TRAP::Network::IPv6Address::Resolve("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") == TRAP::Network::IPv6Address(arr));
    }

    SECTION("TRAP::Network::IPv6Address::operator!=()")
    {
        REQUIRE(TRAP::Network::IPv6Address::Resolve("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") != TRAP::Network::IPv6Address::Resolve("1234"));
        static constexpr std::array<u8, 16u> arr1{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC1u};
        static constexpr std::array<u8, 16u> arr{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC0u};
        STATIC_REQUIRE(TRAP::Network::IPv6Address(arr) != TRAP::Network::IPv6Address(arr1));
    }

    SECTION("TRAP::Network::IPv6Address::operator<()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) < TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}));

    }

    SECTION("TRAP::Network::IPv6Address::operator<=()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) <= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}));

    }

    SECTION("TRAP::Network::IPv6Address::operator>=()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}) >= TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));

    }

    SECTION("TRAP::Network::IPv6Address::operator>()")
    {
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));
        STATIC_REQUIRE(TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u}) > TRAP::Network::IPv6Address(std::array<u8, 16u>{1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u}));

    }

    SECTION("TRAP::Network::IPv6Address::operator>>()")
    {
        TRAP::Optional<TRAP::Network::IPv6Address> ipAddress{};
        std::istringstream("2a02:908:2226:1220:a2e6:22c4:86bd:20c0") >> ipAddress;
        REQUIRE(ipAddress);
        REQUIRE(ipAddress->ToString() == "2a02:908:2226:1220:a2e6:22c4:86bd:20c0");
        static constexpr std::array<u8, 16u> arr{0x2Au, 0x02u, 0x09u, 0x08u, 0x22u, 0x26u, 0x12u, 0x20u, 0xA2u, 0xE6u, 0x22u, 0xC4u, 0x86u, 0xBDu, 0x20u, 0xC0u};
        REQUIRE(ipAddress->ToArray() == arr);

        TRAP::Optional<TRAP::Network::IPv6Address> ipAddress2{};
        std::istringstream("::1") >> ipAddress2;
        REQUIRE(ipAddress2);
        REQUIRE(ipAddress2->ToString() == "::1");
        static constexpr std::array<u8, 16u> arr2{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u};
        REQUIRE(ipAddress2->ToArray() == arr2);

        TRAP::Optional<TRAP::Network::IPv6Address> ipAddress3{};
        std::istringstream("") >> ipAddress3;
        REQUIRE_FALSE(ipAddress3);
    }
}
