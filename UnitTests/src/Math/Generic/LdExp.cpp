#include <cstdint>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::LdExp()", "[math][generic][ldexp][scalar]", f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<std::tuple<TestType, i32, TestType>, 4u> values
        {
            std::tuple(TestType(0.5f), 11, TestType(1024.0f)),
            std::tuple(TestType(0.96f), -2, TestType(0.24f)),
            std::tuple(TestType(0.0f), 0, TestType(0.0f)),
            std::tuple(TestType(-0.665f), 1, TestType(-1.33f)),
        };

        for(const auto& [x, exp, expected] : values)
        {
            const TestType res = TRAP::Math::LdExp(x, exp);
            REQUIRE(TRAP::Math::Equal(res, expected, Epsilon));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
        static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
        static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType min = std::numeric_limits<TestType>::denorm_min();
        static constexpr TestType nmin = -std::numeric_limits<TestType>::denorm_min();

        TestType res{};

        res = TRAP::Math::LdExp(TestType(0.0f), 5);
        REQUIRE(TRAP::Math::Equal(res, TestType(0.0f), Epsilon));

        res = TRAP::Math::LdExp(inf, 10);
        REQUIRE(TRAP::Math::IsInf(res));

        res = TRAP::Math::LdExp(ninf, 10);
        REQUIRE(TRAP::Math::IsInf(res));

        res = TRAP::Math::LdExp(nan, 0);
        REQUIRE(TRAP::Math::IsNaN(res));

        if constexpr(std::same_as<TestType, f64>)
        {
            res = TRAP::Math::LdExp(max, 1);
            REQUIRE(TRAP::Math::IsInf(res));

            res = TRAP::Math::LdExp(min, 1);
            REQUIRE(TRAP::Math::Equal(res, min * 2, Epsilon));

            res = TRAP::Math::LdExp(nmin, -0);
            REQUIRE(TRAP::Math::Equal(res, nmin, Epsilon));
        }
        else if constexpr(std::same_as<TestType, f32>)
        {
            res = TRAP::Math::LdExp(max, 1);
            REQUIRE(TRAP::Math::IsInf(res));

            res = TRAP::Math::LdExp(min, 1);
            REQUIRE(TRAP::Math::Equal(res, min * 2, Epsilon));

            res = TRAP::Math::LdExp(nmin, -0);
            REQUIRE(TRAP::Math::Equal(res, nmin, Epsilon));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::LdExp()", "[math][generic][ldexp][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    static constexpr TestType x(Vec4Scalar(0.5f, 0.96f, 0.0f, -0.665f));
    static constexpr TRAP::Math::Vec<TestType::Length(), i32> exp(TRAP::Math::Vec<4, i32>(11, -2, 0, 1));
    const TestType res = TRAP::Math::LdExp(x, exp);
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TestType(Vec4Scalar(1024.0f, 0.24f, 0.0f, -1.33f)), Epsilon)));
}
