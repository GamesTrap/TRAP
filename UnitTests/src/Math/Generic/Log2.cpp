#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Log2()", "[math][generic][log2][scalar]", f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Log2(TestType(0.5f)), TestType(-1.0f), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log2(TestType(1.0f)), TestType(0.0f), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log2(TestType(1.5f)), TestType(0.584962f), TestType(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log2(TestType(41.5f)), TestType(5.37504f), TestType(0.000001f)));
    }

    SECTION("Normal cases - std")
    {
        static constexpr std::array<std::tuple<TestType, TestType>, 4u> values
        {
            std::tuple(TestType(0.5f), TestType(-1.0f)),
            std::tuple(TestType(1.0f), TestType(0.0f)),
            std::tuple(TestType(1.5f), TestType(0.584962f)),
            std::tuple(TestType(41.5f), TestType(5.37504f))
        };

        for(const auto& [x, expected] : values)
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Log2(x), expected, TestType(0.000001f)));
    }

    SECTION("Edge cases")
    {
        if constexpr(std::floating_point<TestType>)
        {
            static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
            static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
            static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
            static constexpr TestType min = std::numeric_limits<TestType>::lowest();

            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Log2(TestType(0.0f))));
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log2(min)));
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Log2(inf)));
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log2(ninf)));
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log2(nan)));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Log2()", "[math][generic][log2][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType x(Vec4Scalar(16.0f, 8.0f, 4.0f, 2.0f));
        static constexpr TestType res(Vec4Scalar(4.0f, 3.0f, 2.0f, 1.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log2(x), res, TestType(0.01f))));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType x(Vec4Scalar(16.0f, 8.0f, 4.0f, 2.0f));
        static constexpr TestType res(Vec4Scalar(4.0f, 3.0f, 2.0f, 1.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log2(x), res, TestType(0.01f))));
    }
}
