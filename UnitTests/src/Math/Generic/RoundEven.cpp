#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::RoundEven()", "[math][generic][roundeven][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.5f)), TestType(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.5f)), TestType( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.5f)), TestType(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.5f)), TestType( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-3.5f)), TestType(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 3.5f)), TestType( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-4.5f)), TestType(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 4.5f)), TestType( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-5.5f)), TestType(-6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 5.5f)), TestType( 6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-6.5f)), TestType(-6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 6.5f)), TestType( 6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-7.5f)), TestType(-8.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 7.5f)), TestType( 8.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.4f)), TestType(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.4f)), TestType( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.6f)), TestType(-3.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.6f)), TestType( 3.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.0f)), TestType(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.0f)), TestType( 2.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 0.0f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 0.5f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.0f)), TestType( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 0.1f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 0.9f)), TestType( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.5f)), TestType( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.9f)), TestType( 2.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-0.0f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-0.5f)), TestType(-0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.0f)), TestType(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-0.1f)), TestType( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-0.9f)), TestType(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.5f)), TestType(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.9f)), TestType(-2.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.5f)), TestType( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.5f)), TestType( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 3.5f)), TestType( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 4.5f)), TestType( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 5.5f)), TestType( 6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 6.5f)), TestType( 6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 7.5f)), TestType( 8.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.5f)), TestType(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.5f)), TestType(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-3.5f)), TestType(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-4.5f)), TestType(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-5.5f)), TestType(-6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-6.5f)), TestType(-6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-7.5f)), TestType(-8.0f), Epsilon));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<std::pair<TestType, TestType>, 48u> values
        {
            std::pair<TestType, TestType>{TestType(-1.5f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType( 1.5f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType(-2.5f), TestType(-2.0f)},
            std::pair<TestType, TestType>{TestType( 2.5f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType(-3.5f), TestType(-4.0f)}, std::pair<TestType, TestType>{TestType( 3.5f), TestType( 4.0f)},
            std::pair<TestType, TestType>{TestType(-4.5f), TestType(-4.0f)}, std::pair<TestType, TestType>{TestType( 4.5f), TestType( 4.0f)}, std::pair<TestType, TestType>{TestType(-5.5f), TestType(-6.0f)},
            std::pair<TestType, TestType>{TestType( 5.5f), TestType( 6.0f)}, std::pair<TestType, TestType>{TestType(-6.5f), TestType(-6.0f)}, std::pair<TestType, TestType>{TestType( 6.5f), TestType( 6.0f)},
            std::pair<TestType, TestType>{TestType(-7.5f), TestType(-8.0f)}, std::pair<TestType, TestType>{TestType( 7.5f), TestType( 8.0f)},

            std::pair<TestType, TestType>{TestType(-2.4f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType( 2.4f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType(-2.6f), TestType(-3.0f)},
            std::pair<TestType, TestType>{TestType( 2.6f), TestType( 3.0f)}, std::pair<TestType, TestType>{TestType(-2.0f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType( 2.0f), TestType( 2.0f)},

            std::pair<TestType, TestType>{TestType( 0.0f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType( 0.5f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType( 1.0f), TestType( 1.0f)},
            std::pair<TestType, TestType>{TestType( 0.1f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType( 0.9f), TestType( 1.0f)}, std::pair<TestType, TestType>{TestType( 1.5f), TestType( 2.0f)},
            std::pair<TestType, TestType>{TestType( 1.9f), TestType( 2.0f)},

            std::pair<TestType, TestType>{TestType(-0.0f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType(-0.5f), TestType(-0.0f)}, std::pair<TestType, TestType>{TestType(-1.0f), TestType(-1.0f)},
            std::pair<TestType, TestType>{TestType(-0.1f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType(-0.9f), TestType(-1.0f)}, std::pair<TestType, TestType>{TestType(-1.5f), TestType(-2.0f)},
            std::pair<TestType, TestType>{TestType(-1.9f), TestType(-2.0f)},

            std::pair<TestType, TestType>{TestType( 1.5f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType( 2.5f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType( 3.5f), TestType( 4.0f)},
            std::pair<TestType, TestType>{TestType( 4.5f), TestType( 4.0f)}, std::pair<TestType, TestType>{TestType( 5.5f), TestType( 6.0f)}, std::pair<TestType, TestType>{TestType( 6.5f), TestType( 6.0f)},
            std::pair<TestType, TestType>{TestType( 7.5f), TestType( 8.0f)},

            std::pair<TestType, TestType>{TestType(-1.5f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType(-2.5f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType(-3.5f), TestType(-4.0f)},
            std::pair<TestType, TestType>{TestType(-4.5f), TestType(-4.0f)}, std::pair<TestType, TestType>{TestType(-5.5f), TestType(-6.0f)}, std::pair<TestType, TestType>{TestType(-6.5f), TestType(-6.0f)},
            std::pair<TestType, TestType>{TestType(-7.5f), TestType(-8.0f)},
        };

        for(const auto& [val, expected] : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::RoundEven(val), expected, Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::RoundEven(max), max, Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::RoundEven(min), min, Epsilon));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::RoundEven()", "[math][generic][roundeven][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.5f)), TestType(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.5f)), TestType( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.5f)), TestType(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.5f)), TestType( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-3.5f)), TestType(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 3.5f)), TestType( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-4.5f)), TestType(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 4.5f)), TestType( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-5.5f)), TestType(-6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 5.5f)), TestType( 6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-6.5f)), TestType(-6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 6.5f)), TestType( 6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-7.5f)), TestType(-8.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 7.5f)), TestType( 8.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.4f)), TestType(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.4f)), TestType( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.6f)), TestType(-3.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.6f)), TestType( 3.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.0f)), TestType(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.0f)), TestType( 2.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 0.0f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 0.5f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.0f)), TestType( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 0.1f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 0.9f)), TestType( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.5f)), TestType( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.9f)), TestType( 2.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-0.0f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-0.5f)), TestType(-0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.0f)), TestType(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-0.1f)), TestType( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-0.9f)), TestType(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.5f)), TestType(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.9f)), TestType(-2.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 1.5f)), TestType( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 2.5f)), TestType( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 3.5f)), TestType( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 4.5f)), TestType( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 5.5f)), TestType( 6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 6.5f)), TestType( 6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType( 7.5f)), TestType( 8.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-1.5f)), TestType(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-2.5f)), TestType(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-3.5f)), TestType(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-4.5f)), TestType(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-5.5f)), TestType(-6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-6.5f)), TestType(-6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(TestType(-7.5f)), TestType(-8.0f), Epsilon)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<std::pair<TestType, TestType>, 48u> values
        {
            std::pair<TestType, TestType>{TestType(-1.5f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType( 1.5f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType(-2.5f), TestType(-2.0f)},
            std::pair<TestType, TestType>{TestType( 2.5f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType(-3.5f), TestType(-4.0f)}, std::pair<TestType, TestType>{TestType( 3.5f), TestType( 4.0f)},
            std::pair<TestType, TestType>{TestType(-4.5f), TestType(-4.0f)}, std::pair<TestType, TestType>{TestType( 4.5f), TestType( 4.0f)}, std::pair<TestType, TestType>{TestType(-5.5f), TestType(-6.0f)},
            std::pair<TestType, TestType>{TestType( 5.5f), TestType( 6.0f)}, std::pair<TestType, TestType>{TestType(-6.5f), TestType(-6.0f)}, std::pair<TestType, TestType>{TestType( 6.5f), TestType( 6.0f)},
            std::pair<TestType, TestType>{TestType(-7.5f), TestType(-8.0f)}, std::pair<TestType, TestType>{TestType( 7.5f), TestType( 8.0f)},

            std::pair<TestType, TestType>{TestType(-2.4f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType( 2.4f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType(-2.6f), TestType(-3.0f)},
            std::pair<TestType, TestType>{TestType( 2.6f), TestType( 3.0f)}, std::pair<TestType, TestType>{TestType(-2.0f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType( 2.0f), TestType( 2.0f)},

            std::pair<TestType, TestType>{TestType( 0.0f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType( 0.5f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType( 1.0f), TestType( 1.0f)},
            std::pair<TestType, TestType>{TestType( 0.1f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType( 0.9f), TestType( 1.0f)}, std::pair<TestType, TestType>{TestType( 1.5f), TestType( 2.0f)},
            std::pair<TestType, TestType>{TestType( 1.9f), TestType( 2.0f)},

            std::pair<TestType, TestType>{TestType(-0.0f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType(-0.5f), TestType(-0.0f)}, std::pair<TestType, TestType>{TestType(-1.0f), TestType(-1.0f)},
            std::pair<TestType, TestType>{TestType(-0.1f), TestType( 0.0f)}, std::pair<TestType, TestType>{TestType(-0.9f), TestType(-1.0f)}, std::pair<TestType, TestType>{TestType(-1.5f), TestType(-2.0f)},
            std::pair<TestType, TestType>{TestType(-1.9f), TestType(-2.0f)},

            std::pair<TestType, TestType>{TestType( 1.5f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType( 2.5f), TestType( 2.0f)}, std::pair<TestType, TestType>{TestType( 3.5f), TestType( 4.0f)},
            std::pair<TestType, TestType>{TestType( 4.5f), TestType( 4.0f)}, std::pair<TestType, TestType>{TestType( 5.5f), TestType( 6.0f)}, std::pair<TestType, TestType>{TestType( 6.5f), TestType( 6.0f)},
            std::pair<TestType, TestType>{TestType( 7.5f), TestType( 8.0f)},

            std::pair<TestType, TestType>{TestType(-1.5f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType(-2.5f), TestType(-2.0f)}, std::pair<TestType, TestType>{TestType(-3.5f), TestType(-4.0f)},
            std::pair<TestType, TestType>{TestType(-4.5f), TestType(-4.0f)}, std::pair<TestType, TestType>{TestType(-5.5f), TestType(-6.0f)}, std::pair<TestType, TestType>{TestType(-6.5f), TestType(-6.0f)},
            std::pair<TestType, TestType>{TestType(-7.5f), TestType(-8.0f)},
        };

        for(const auto& [val, expected] : values)
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(val), TestType(expected), Epsilon)));
    }
}
