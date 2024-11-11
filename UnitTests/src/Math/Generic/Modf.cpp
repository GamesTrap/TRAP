#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Modf()", "[math][generic][modf][scalar]", f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();
        TestType i{};

        static constexpr std::array<std::tuple<TestType, TestType, TestType>, 1u> values
        {
            std::tuple{TestType(1.5), TestType(0.5), TestType(1.0)}
        };

        for(const auto& [val, expected, expectedI] : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Modf(val, i), expected, Epsilon));
            REQUIRE(TRAP::Math::Equal(i, expectedI, Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();

        TestType i{};

        const TestType resMax = TRAP::Math::Modf(max, i);
        REQUIRE((TRAP::Math::Equal(resMax, TestType(0), Epsilon) && (static_cast<TestType>(i) == max)));

        const TestType resMin = TRAP::Math::Modf(min, i);
        REQUIRE((TRAP::Math::Equal(resMin, TestType(0), Epsilon) && (static_cast<TestType>(i) == min)));

        const TestType resNaN = TRAP::Math::Modf(nan, i);
        REQUIRE((TRAP::Math::IsNaN(resNaN) && TRAP::Math::IsNaN(i)));

        const TestType resInf = TRAP::Math::Modf(inf, i);
        REQUIRE((TRAP::Math::Equal(resInf, TestType(0), Epsilon) && (static_cast<TestType>(i) == inf)));

        const TestType resNInf = TRAP::Math::Modf(-inf, i);
        REQUIRE((TRAP::Math::Equal(resNInf, TestType(-0), Epsilon) && (static_cast<TestType>(i) == -inf)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Modf()", "[math][generic][modf][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();
    TestType i{};

    static constexpr std::array<std::tuple<TestType, TestType, TestType>, 1u> values
    {
        std::tuple<TestType, TestType, TestType>{TRAP::Math::Vec4d{1.1, 1.2, 1.5, 1.7}, TRAP::Math::Vec4d{0.1, 0.2, 0.5, 0.7}, TRAP::Math::Vec4d(1.0)}
    };

    for(const auto& [val, expected, expectedI] : values)
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Modf(val, i), expected, Epsilon)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(i, expectedI, Epsilon)));
    }
}
