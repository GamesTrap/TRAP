#include <cstdint>
#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Mod()", "[math][generic][mod][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        if constexpr(std::floating_point<TestType>)
        {
            static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();
            static_assert(TRAP::Math::Equal(TRAP::Math::Mod(TestType( 1.5f), TestType(1.0f)), TestType( 0.5f), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Mod(TestType(-0.2f), TestType(1.0f)), TestType(-0.2f), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Mod(TestType( 3.0f), TestType(2.0f)), TestType( 1.0f), Epsilon));
        }
        else if constexpr(std::integral<TestType>)
        {
            static_assert(TRAP::Math::Mod(TestType( 1.5f), TestType(1.0f)) == TestType( 0.5f));
            static_assert(TRAP::Math::Mod(TestType(-0.2f), TestType(1.0f)) == TestType(-0.2f));
            static_assert(TRAP::Math::Mod(TestType( 3.0f), TestType(2.0f)) == TestType( 1.0f));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<std::tuple<TestType, TestType, TestType>, 3u> values
        {
            std::tuple<TestType, TestType, TestType>{TestType( 1.5f), TestType(1.0f), TestType( 0.5f)},
            std::tuple<TestType, TestType, TestType>{TestType(-0.2f), TestType(1.0f), TestType(-0.2f)},
            std::tuple<TestType, TestType, TestType>{TestType( 3.0f), TestType(2.0f), TestType( 1.0f)},
        };

        for(const auto& [val1, val2, expected] : values)
        {
            if constexpr(std::floating_point<TestType>)
            {
                static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Mod(val1, val2), expected, Epsilon));
            }
            else if constexpr(std::integral<TestType>)
            {
                REQUIRE(TRAP::Math::Mod(val1, val2) == expected);
            }
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();

        if constexpr(std::floating_point<TestType>)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Mod(max, TestType(100.0)), static_cast<TestType>(std::fmod(max, TestType(100))), Epsilon));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Mod(min, TestType(-50.0)), static_cast<TestType>(std::fmod(min, TestType(-50.0))), Epsilon));

            static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Mod(TestType(15), TestType(0))));
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Mod(TestType(2.0), nan)));
        }
        else if constexpr(std::integral<TestType>)
        {
            REQUIRE(TRAP::Math::Mod(max, TestType(100.0)) == static_cast<TestType>(max % TestType(100)));

            if constexpr(std::signed_integral<TestType>)
            {
                REQUIRE(TRAP::Math::Mod(min, TestType(-50.0)) == static_cast<TestType>(min % TestType(-50.0)));
            }
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Mod()", "[math][generic][mod][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    SECTION("Normal cases - GCEM")
    {
        {
            constexpr typename TestType::value_type Epsilon = std::numeric_limits<typename TestType::value_type>::epsilon();
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(TestType( 1.5f), TestType(1.0f)), TestType( 0.5f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(TestType(-0.2f), TestType(1.0f)), TestType(-0.2f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(TestType( 3.0f), TestType(2.0f)), TestType( 1.0f), Epsilon)));
        }
        {
            using X = typename TestType::value_type;

            constexpr typename TestType::value_type Epsilon = std::numeric_limits<typename TestType::value_type>::epsilon();
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(TestType( 1.5f), X(1.0f)), TestType( 0.5f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(TestType(-0.2f), X(1.0f)), TestType(-0.2f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(TestType( 3.0f), X(2.0f)), TestType( 1.0f), Epsilon)));
        }
    }

    SECTION("Normal cases - std")
    {
        {
            static constexpr std::array<std::tuple<TestType, TestType, TestType>, 3u> values
            {
                std::tuple<TestType, TestType, TestType>{TestType( 1.5f), TestType(1.0f), TestType( 0.5f)},
                std::tuple<TestType, TestType, TestType>{TestType(-0.2f), TestType(1.0f), TestType(-0.2f)},
                std::tuple<TestType, TestType, TestType>{TestType( 3.0f), TestType(2.0f), TestType( 1.0f)},
            };

            static constexpr typename TestType::value_type Epsilon = std::numeric_limits<typename TestType::value_type>::epsilon();
            for(const auto& [val1, val2, expected] : values)
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(val1, TestType(val2)), expected, Epsilon)));
        }
        {
            using X = typename TestType::value_type;

            static constexpr std::array<std::tuple<TestType, X, TestType>, 3u> values
            {
                std::tuple<TestType, X, TestType>{TestType( 1.5f), X(1.0f), TestType( 0.5f)},
                std::tuple<TestType, X, TestType>{TestType(-0.2f), X(1.0f), TestType(-0.2f)},
                std::tuple<TestType, X, TestType>{TestType( 3.0f), X(2.0f), TestType( 1.0f)},
            };

            for(const auto& [val1, val2, expected] : values)
            {
                constexpr typename TestType::value_type Epsilon = std::numeric_limits<typename TestType::value_type>::epsilon();
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(val1, val2), expected, Epsilon)));
            }
        }
    }
}
