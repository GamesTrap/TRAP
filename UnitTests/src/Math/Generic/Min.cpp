#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Min()", "[math][generic]", int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, double, float)
{
    constexpr TestType NaN = std::numeric_limits<TestType>::quiet_NaN();

    SECTION("Scalar")
    {
        constexpr TestType N = static_cast<TestType>(0);
        constexpr TestType B = static_cast<TestType>(1);
        constexpr bool a0 = TRAP::Math::Equal(TRAP::Math::Min(N, B), N, TRAP::Math::Epsilon<TestType>());
        constexpr bool a1 = TRAP::Math::Equal(TRAP::Math::Min(B, N), N, TRAP::Math::Epsilon<TestType>());
        static_assert(a0, "TRAP::Math::Min(N, B) == N failed");
        static_assert(a1, "TRAP::Math::Min(B, N) == N failed");
        REQUIRE(a0 == true);
        REQUIRE(a1 == true);
    }

    SECTION("NaN")
    {
        if constexpr(std::same_as<TestType, float> || std::same_as<TestType, double>)
        {
            constexpr TestType B = static_cast<TestType>(1);
            constexpr TestType N = static_cast<TestType>(NaN);
            constexpr bool a0 = TRAP::Math::IsNaN(TRAP::Math::Min(N, B));
            constexpr bool a1 = !TRAP::Math::IsNaN(TRAP::Math::Min(B, N));
            static_assert(a0, "TRAP::Math::IsNaN(Min(N, B)) failed");
            static_assert(a1, "!TRAP::Math::IsNaN(Min(N, B)) failed");
            REQUIRE(a0 == true);
            REQUIRE(a1 == true);
        }
    }

    SECTION("Vec2")
    {
        using Vec2T = TRAP::Math::Vec<2, TestType>;

        constexpr Vec2T a0 = TRAP::Math::Min(Vec2T(static_cast<TestType>(1)), Vec2T(static_cast<TestType>(1)));
        constexpr Vec2T a1 = TRAP::Math::Min(Vec2T(static_cast<TestType>(1)), static_cast<TestType>(1));
        constexpr bool a2 = TRAP::Math::All(TRAP::Math::Equal(a0, a1, TRAP::Math::Epsilon<TestType>()));
        static_assert(a2);
        REQUIRE(a2 == true);
    }

    SECTION("Vec3")
    {
        using Vec3T = TRAP::Math::Vec<3, TestType>;

        constexpr Vec3T a0 = TRAP::Math::Min(Vec3T(static_cast<TestType>(1)), Vec3T(static_cast<TestType>(1)));
        constexpr Vec3T a1 = TRAP::Math::Min(Vec3T(static_cast<TestType>(1)), static_cast<TestType>(1));
        constexpr bool a2 = TRAP::Math::All(TRAP::Math::Equal(a0, a1, TRAP::Math::Epsilon<TestType>()));
        static_assert(a2);
        REQUIRE(a2 == true);
    }

    SECTION("Vec4")
    {
        using Vec4T = TRAP::Math::Vec<4, TestType>;

        constexpr Vec4T a0 = TRAP::Math::Min(Vec4T(static_cast<TestType>(1)), Vec4T(static_cast<TestType>(1)));
        constexpr Vec4T a1 = TRAP::Math::Min(Vec4T(static_cast<TestType>(1)), static_cast<TestType>(1));
        constexpr bool a2 = TRAP::Math::All(TRAP::Math::Equal(a0, a1, TRAP::Math::Epsilon<TestType>()));
        static_assert(a2);
        REQUIRE(a2 == true);
    }
}