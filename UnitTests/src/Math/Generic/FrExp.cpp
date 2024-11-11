#include <cstdint>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::FrExp()", "[math][generic][frexp][scalar]", f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        static constexpr std::array<std::tuple<TestType, TestType, i32>, 4u> values
        {
            std::tuple(TestType(1024.0f), TestType(0.5f), 11),
            std::tuple(TestType(0.24f), TestType(0.96f), -2),
            std::tuple(TestType(0.0f), TestType(0.0f), 0),
            std::tuple(TestType(-1.33f), TestType(-0.665f), 1),
        };

        for(const auto& [x, expectedRes, expectedExp] : values)
        {
            i32 exp{};
            const TestType res = TRAP::Math::FrExp(x, exp);
            REQUIRE(TRAP::Math::Equal(res, expectedRes, Epsilon));
            REQUIRE(exp == expectedExp);
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

        i32 exp{};
        TestType res{};

        res = TRAP::Math::FrExp(TestType(0.0f), exp);
        REQUIRE(TRAP::Math::Equal(res, TestType(0.0f), Epsilon));
        REQUIRE(exp == 0);

        res = TRAP::Math::FrExp(inf, exp);
        REQUIRE(TRAP::Math::IsInf(res));

        res = TRAP::Math::FrExp(ninf, exp);
        REQUIRE(TRAP::Math::IsInf(res));

        res = TRAP::Math::FrExp(nan, exp);
        REQUIRE(TRAP::Math::IsNaN(res));

        if constexpr(std::same_as<TestType, f64>)
        {
            res = TRAP::Math::FrExp(max, exp);
            REQUIRE(TRAP::Math::Equal(res, TestType(0.99999999999999989f), Epsilon));
            REQUIRE(exp == 1024);

            res = TRAP::Math::FrExp(min, exp);
            REQUIRE(TRAP::Math::Equal(res, TestType(0.5f), Epsilon));
            REQUIRE(exp == -1073);

            res = TRAP::Math::FrExp(nmin, exp);
            REQUIRE(TRAP::Math::Equal(res, TestType(-0.5f), Epsilon));
            REQUIRE(exp == -1073);
        }
        else if constexpr(std::same_as<TestType, f32>)
        {
            res = TRAP::Math::FrExp(max, exp);
            REQUIRE(TRAP::Math::Equal(res, TestType(0.99999999999999989f), Epsilon));
            REQUIRE(exp == 128);

            res = TRAP::Math::FrExp(min, exp);
            REQUIRE(TRAP::Math::Equal(res, TestType(0.5f), Epsilon));
            REQUIRE(exp == -148);

            res = TRAP::Math::FrExp(nmin, exp);
            REQUIRE(TRAP::Math::Equal(res, TestType(-0.5f), Epsilon));
            REQUIRE(exp == -148);
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::FrExp()", "[math][generic][frexp][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    static constexpr TestType x(Vec4Scalar(1024.0f, 0.24f, 0.0f, -1.33f));
    TRAP::Math::Vec<TestType::Length(), i32> exp{};
    const TestType res = TRAP::Math::FrExp(x, exp);
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TestType(Vec4Scalar(0.5f, 0.96f, 0.0f, -0.665f)), Epsilon)));
    REQUIRE(exp == TRAP::Math::Vec<TestType::Length(), i32>(TRAP::Math::Vec<4u, i32>(11, -2, 0, 1)));
}
