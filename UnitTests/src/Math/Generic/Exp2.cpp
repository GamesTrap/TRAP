#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Exp2()", "[math][generic][exp2][scalar]", f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr std::array<std::tuple<TestType, TestType>, 6u> values
        {
            std::tuple(TestType(23.0f), TestType(8388608.f)),
            std::tuple(TestType(2.5f), TestType(5.65685f)),
            std::tuple(TestType(-1.75f), TestType(0.297302f)),
            std::tuple(TestType(0.0f), TestType(1.0f)),
            std::tuple(TestType(10.0f), TestType(1024.0f)),
            std::tuple(TestType(-5.0f), TestType(0.03125f)),
        };

        for(const auto& [num, expected] : values)
        {
            const TestType res = TRAP::Math::Exp2(num);
            REQUIRE(TRAP::Math::Equal(res, expected, TestType(0.00001f)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::lowest();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Exp2(nan)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp2(inf)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp2(ninf), TestType(0.0f), Epsilon));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp2(max)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp2(min), TestType(0.0f), Epsilon));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Exp2()", "[math][generic][exp2][vec]",
                   TRAP::Math::Vec2d, TRAP::Math::Vec2f, TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    static constexpr TestType x(Vec4Scalar(4.0f, 3.0f, 2.0f, 1.0f));
    static constexpr TestType res(Vec4Scalar(16.0f, 8.0f, 4.0f, 2.0f));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp2(x), res, TestType(0.01f))));
}
